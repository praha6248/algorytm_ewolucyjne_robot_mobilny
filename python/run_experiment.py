"""Seria uruchomien z pliku konfiguracyjnego + wykresy.

    python python/run_experiment.py configs/default.json
"""

import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # zapis do plikow bez okna
import matplotlib.pyplot as plt

import evopath
from evopath import plots
from evopath.experiments import load_json


def main(config_path: str) -> None:
    params = load_json(config_path)
    name = params.get("name", Path(config_path).stem)
    out = Path("results") / name
    out.mkdir(parents=True, exist_ok=True)

    map_obj = evopath.Map.load(params["map"])
    results = evopath.run_series(map_obj, params, params["generations"], range(params["seeds"]))
    stats = evopath.summarize(results)
    print(stats)

    best = min(results, key=lambda r: r["best_fitness"])
    plots.plot_path(map_obj, best["best_path"], title=f"Najlepsza sciezka (seed {best['seed']})")
    plt.savefig(out / "best_path.png", dpi=150, bbox_inches="tight")

    plots.plot_convergence(best["history"])
    plt.savefig(out / "convergence.png", dpi=150, bbox_inches="tight")

    if best["snapshots"]:
        plots.plot_snapshots(map_obj, best["snapshots"][:6])
        plt.savefig(out / "snapshots.png", dpi=150, bbox_inches="tight")

    print(f"Wykresy zapisane w {out}/")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "configs/default.json")
