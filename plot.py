"""Wykres mapy z najlepsza sciezka i krzywej zbieznosci.

    python3 plot.py [mapa=maps/map1.txt]

Czyta results/best_path.csv i results/history.csv (tworzy je ./build/robot_ga),
zapisuje results/best_path.png i results/convergence.png.
"""

import csv
import sys

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


def load_map(file_name):
    """Ten sam format co Map::load: komentarze '#' tylko przed linia 'size'."""
    rows, start, goal, has_size = [], None, None, False
    with open(file_name) as f:
        for line in f:
            line = line.rstrip("\r\n")
            if not line or (line.startswith("#") and not has_size):
                continue
            key = line.split()[0]
            if key == "size":
                has_size = True
            elif key == "start":
                start = tuple(map(float, line.split()[1:3]))
            elif key == "goal":
                goal = tuple(map(float, line.split()[1:3]))
            else:
                rows.append([c == "#" for c in line])
    return np.array(rows), start, goal


def read_csv(file_name):
    with open(file_name) as f:
        reader = csv.DictReader(f)
        data = [row for row in reader]
    return {k: np.array([float(r[k]) for r in data]) for k in data[0]}


def main():
    map_file = sys.argv[1] if len(sys.argv) > 1 else "maps/map1.txt"
    grid, start, goal = load_map(map_file)
    h, w = grid.shape
    path = read_csv("results/best_path.csv")
    hist = read_csv("results/history.csv")

    fig, ax = plt.subplots(figsize=(9, 9 * h / w))
    ax.imshow(grid, cmap="gray_r", extent=(0, w, h, 0), interpolation="nearest")
    ax.plot(path["x"], path["y"], "-o", color="tab:blue", lw=2, ms=4, label="sciezka")
    ax.plot(*start, "o", color="tab:green", ms=10, label="start")
    ax.plot(*goal, "*", color="tab:red", ms=14, label="cel")
    ax.set_xlim(0, w)
    ax.set_ylim(h, 0)
    ax.set_aspect("equal")
    ax.legend(loc="upper right")
    ax.set_title("Najlepsza sciezka")
    fig.savefig("results/best_path.png", dpi=150, bbox_inches="tight")

    fig, ax = plt.subplots(figsize=(8, 4.5))
    ax.plot(hist["generation"], hist["best"], label="najlepszy", lw=2)
    ax.plot(hist["generation"], hist["mean"], label="sredni")
    ax.set_yscale("log")
    ax.set_xlabel("pokolenie")
    ax.set_ylabel("funkcja celu (mniej = lepiej)")
    ax.grid(alpha=0.3)
    ax.legend()
    ax.set_title("Zbieznosc")
    fig.savefig("results/convergence.png", dpi=150, bbox_inches="tight")
    print("Zapisano results/best_path.png i results/convergence.png")


if __name__ == "__main__":
    main()
