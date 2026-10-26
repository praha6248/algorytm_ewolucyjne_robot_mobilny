"""Przyklad uzycia biblioteki: jedno uruchomienie, seria 30 uruchomien, wykresy.

    PYTHONPATH=python python3 python/example.py        (po cmake --build build)
    python3 python/example.py                          (po pip install .)
"""

from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

import robot_ga
from robot_ga import plots

out = Path("results")
out.mkdir(exist_ok=True)

m = robot_ga.Map("maps/map1.txt")

# 1. jedno uruchomienie z migawkami populacji
result = robot_ga.run(m, generations=300, max_path=8, seed=1, snapshot_every=50)
print(f"najlepszy = {result['best_value']:.2f}, kolizje = {result['best_collision']:.2f}, "
      f"czas = {result['time_s']:.2f} s")

plots.plot_path(m, result["best_path"])
plt.savefig(out / "best_path.png", dpi=150, bbox_inches="tight")

plots.plot_convergence(result["history"])
plt.savefig(out / "convergence.png", dpi=150, bbox_inches="tight")

plots.plot_snapshots(m, result["snapshots"])
plt.savefig(out / "snapshots.png", dpi=150, bbox_inches="tight")

# 2. seria 30 uruchomien z innym prawdopodobienstwem mutacji
series = robot_ga.run_series(m, seeds=range(30), generations=300, mutation_prob=0.15)
print(robot_ga.summarize(series))

print(f"Wykresy zapisane w {out}/")
