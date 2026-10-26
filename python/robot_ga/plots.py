"""Wykresy: mapa, sciezka, zbieznosc, populacja w wybranych pokoleniach, animacja."""

from __future__ import annotations

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation


def plot_map(map_obj, ax=None):
    """Mapa (przeszkody na czarno) ze startem i celem."""
    if ax is None:
        _, ax = plt.subplots(figsize=(9, 9 * map_obj.height / max(map_obj.width, 1)))
    ax.imshow(map_obj.grid, cmap="gray_r", extent=(0, map_obj.width, map_obj.height, 0),
              interpolation="nearest")
    ax.plot(*map_obj.start, "o", color="tab:green", ms=10, label="start", zorder=5)
    ax.plot(*map_obj.goal, "*", color="tab:red", ms=14, label="cel", zorder=5)
    ax.set_xlim(0, map_obj.width)
    ax.set_ylim(map_obj.height, 0)
    ax.set_aspect("equal")
    return ax


def plot_path(map_obj, path, ax=None, title="Najlepsza sciezka"):
    """Mapa + pelna sciezka (tablica (n, 2), np. result["best_path"])."""
    ax = plot_map(map_obj, ax)
    path = np.asarray(path)
    ax.plot(path[:, 0], path[:, 1], "-o", color="tab:blue", lw=2, ms=4, label="sciezka")
    ax.legend(loc="upper right")
    if title:
        ax.set_title(title)
    return ax


def plot_convergence(history, ax=None, log=True, title="Zbieznosc"):
    """Najlepsza i srednia wartosc funkcji celu w kolejnych pokoleniach."""
    if ax is None:
        _, ax = plt.subplots(figsize=(8, 4.5))
    ax.plot(history["generation"], history["best"], label="najlepszy", lw=2)
    ax.plot(history["generation"], history["mean"], label="sredni")
    if log:
        ax.set_yscale("log")
    ax.set_xlabel("pokolenie")
    ax.set_ylabel("funkcja celu (mniej = lepiej)")
    ax.grid(alpha=0.3)
    ax.legend()
    ax.set_title(title)
    return ax


def _draw_individuals(ax, map_obj, individuals, alpha=0.15):
    lines = []
    for mid in individuals:
        pts = np.vstack([map_obj.start, np.asarray(mid).reshape(-1, 2), map_obj.goal])
        (ln,) = ax.plot(pts[:, 0], pts[:, 1], color="tab:blue", lw=0.8, alpha=alpha)
        lines.append(ln)
    return lines


def plot_population(map_obj, individuals, ax=None, title=None, alpha=0.15):
    """Wszystkie sciezki populacji naraz (np. pop.individuals())."""
    ax = plot_map(map_obj, ax)
    _draw_individuals(ax, map_obj, individuals, alpha)
    if title:
        ax.set_title(title)
    return ax


def plot_snapshots(map_obj, snapshots, cols=3):
    """Siatka wykresow: populacja w kolejnych migawkach (run(..., snapshot_every=N))."""
    if not snapshots:
        raise ValueError("Brak migawek - uruchom run(..., snapshot_every=N)")
    rows = int(np.ceil(len(snapshots) / cols))
    fig, axes = plt.subplots(rows, cols, figsize=(5 * cols, 3.6 * rows), squeeze=False)
    for ax, snap in zip(axes.flat, snapshots):
        plot_population(map_obj, snap["individuals"], ax, f"pokolenie {snap['generation']}")
    for ax in axes.flat[len(snapshots):]:
        ax.axis("off")
    fig.tight_layout()
    return fig


def animate(map_obj, snapshots, interval=200, alpha=0.15):
    """Animacja populacji. Zapis: anim.save("ewolucja.gif", writer="pillow")."""
    if not snapshots:
        raise ValueError("Brak migawek - uruchom run(..., snapshot_every=N)")
    fig, ax = plt.subplots(figsize=(8, 8 * map_obj.height / max(map_obj.width, 1)))
    plot_map(map_obj, ax)
    state = {"lines": []}

    def update(i):
        for ln in state["lines"]:
            ln.remove()
        state["lines"] = _draw_individuals(ax, map_obj, snapshots[i]["individuals"], alpha)
        ax.set_title(f"pokolenie {snapshots[i]['generation']}")
        return state["lines"]

    return FuncAnimation(fig, update, frames=len(snapshots), interval=interval)
