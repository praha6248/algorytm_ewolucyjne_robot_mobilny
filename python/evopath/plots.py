"""Wykresy: mapa z trasa, zbieznosc, migawki populacji, animacja."""

from __future__ import annotations

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation


def plot_map(map_obj, ax=None):
    """Rysuje mape (przeszkody na czarno) oraz start i cel."""
    if ax is None:
        _, ax = plt.subplots(figsize=(8, 8 * map_obj.height / max(map_obj.width, 1)))
    ax.imshow(map_obj.grid, cmap="gray_r", origin="upper",
              extent=(0, map_obj.width, map_obj.height, 0), interpolation="nearest")
    sx, sy = map_obj.start
    gx, gy = map_obj.goal
    ax.plot(sx, sy, "o", color="tab:green", ms=10, label="start", zorder=5)
    ax.plot(gx, gy, "*", color="tab:red", ms=14, label="cel", zorder=5)
    ax.set_xlim(0, map_obj.width)
    ax.set_ylim(map_obj.height, 0)
    ax.set_aspect("equal")
    return ax


def plot_path(map_obj, path, ax=None, title=None, **style):
    """Mapa + sciezka (tablica (N, 2) z Solver.best_path())."""
    ax = plot_map(map_obj, ax)
    path = np.asarray(path)
    kw = {"color": "tab:blue", "lw": 2, "marker": "o", "ms": 4, "label": "sciezka"}
    kw.update(style)
    ax.plot(path[:, 0], path[:, 1], **kw)
    if title:
        ax.set_title(title)
    ax.legend(loc="upper right")
    return ax


def plot_convergence(history, ax=None, log=False, title="Zbieznosc"):
    """Najlepszy / sredni / najgorszy fitness w kolejnych pokoleniach."""
    if ax is None:
        _, ax = plt.subplots(figsize=(8, 4.5))
    g = history["generation"]
    ax.plot(g, history["best"], label="najlepszy", lw=2)
    ax.plot(g, history["mean"], label="sredni")
    ax.plot(g, history["worst"], label="najgorszy", alpha=0.6)
    ax.set_xlabel("pokolenie")
    ax.set_ylabel("fitness (mniej = lepiej)")
    if log:
        ax.set_yscale("log")
    ax.set_title(title)
    ax.grid(alpha=0.3)
    ax.legend()
    return ax


def _draw_population(ax, map_obj, snapshot, alpha=0.15):
    sx, sy = map_obj.start
    gx, gy = map_obj.goal
    lines = []
    for pts_mid in snapshot["paths"]:
        pts = np.vstack([[sx, sy], np.asarray(pts_mid).reshape(-1, 2), [gx, gy]])
        (ln,) = ax.plot(pts[:, 0], pts[:, 1], color="tab:blue", lw=0.8, alpha=alpha)
        lines.append(ln)
    return lines


def plot_snapshots(map_obj, snapshots, which=None, cols=3):
    """Siatka wykresow: cala populacja w wybranych pokoleniach."""
    if which is not None:
        snapshots = [s for s in snapshots if s["generation"] in set(which)]
    if not snapshots:
        raise ValueError("Brak migawek - ustaw config.snapshot_every > 0")
    rows = int(np.ceil(len(snapshots) / cols))
    fig, axes = plt.subplots(rows, cols, figsize=(4.5 * cols, 4.5 * rows), squeeze=False)
    for ax, snap in zip(axes.flat, snapshots):
        plot_map(map_obj, ax)
        _draw_population(ax, map_obj, snap)
        ax.set_title(f"pokolenie {snap['generation']}")
    for ax in axes.flat[len(snapshots):]:
        ax.axis("off")
    fig.tight_layout()
    return fig


def animate_population(map_obj, snapshots, interval=200, alpha=0.15):
    """Animacja ewolucji populacji. Zapis: anim.save('ewolucja.gif', writer='pillow')."""
    if not snapshots:
        raise ValueError("Brak migawek - ustaw config.snapshot_every > 0")
    fig, ax = plt.subplots(figsize=(7, 7 * map_obj.height / max(map_obj.width, 1)))
    plot_map(map_obj, ax)
    state = {"lines": []}

    def update(frame):
        for ln in state["lines"]:
            ln.remove()
        state["lines"] = _draw_population(ax, map_obj, snapshots[frame], alpha)
        ax.set_title(f"pokolenie {snapshots[frame]['generation']}")
        return state["lines"]

    return FuncAnimation(fig, update, frames=len(snapshots), interval=interval, blit=False)
