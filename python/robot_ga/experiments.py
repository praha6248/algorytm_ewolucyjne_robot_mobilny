"""Uruchamianie algorytmu: pojedyncze przebiegi, serie z roznymi ziarnami, statystyki."""

from __future__ import annotations

import time

import numpy as np

from ._core import Map, Parameters, Population


def make_parameters(**kwargs) -> Parameters:
    """Parameters z argumentow nazwanych, np. make_parameters(pop_size=200, mutation_prob=0.2)."""
    p = Parameters()
    for key, value in kwargs.items():
        if not hasattr(p, key):
            raise KeyError(f"Nieznany parametr: {key}")
        setattr(p, key, value)
    return p


def run(map_obj: Map, generations: int = 300, max_path: int = 8, seed: int = 1,
        params: Parameters | None = None, snapshot_every: int = 0, **kwargs) -> dict:
    """Jedno uruchomienie algorytmu.

    Parametry algorytmu mozna podac jako `params` albo wprost: run(m, mutation_prob=0.2).
    snapshot_every > 0 zapisuje populacje co tyle pokolen (do wykresow i animacji).
    """
    if params is None:
        params = make_parameters(**kwargs)
    elif kwargs:
        raise TypeError("Podaj albo params, albo parametry jako argumenty nazwane")

    pop = Population(map_obj, max_path=max_path, seed=seed, params=params)
    best, mean, snapshots = [pop.best_value()], [pop.mean_value()], []
    if snapshot_every > 0:
        snapshots.append({"generation": 0, "individuals": pop.individuals()})

    t0 = time.perf_counter()
    for g in range(1, generations + 1):
        pop.next_generation()
        best.append(pop.best_value())
        mean.append(pop.mean_value())
        if snapshot_every > 0 and g % snapshot_every == 0:
            snapshots.append({"generation": g, "individuals": pop.individuals()})
    elapsed = time.perf_counter() - t0

    return {
        "seed": seed,
        "best_value": pop.best_value(),
        "best_collision": pop.best_collision(),
        "best_path": pop.best_path(),
        "history": {
            "generation": np.arange(generations + 1),
            "best": np.array(best),
            "mean": np.array(mean),
        },
        "snapshots": snapshots,
        "time_s": elapsed,
        "population": pop,
    }


def run_series(map_obj: Map, seeds=range(30), **kwargs) -> list[dict]:
    """Seria niezaleznych uruchomien z roznymi ziarnami (argumenty jak w run)."""
    return [run(map_obj, seed=int(s), **kwargs) for s in seeds]


def summarize(results: list[dict]) -> dict:
    """Najlepszy, sredni, najgorszy wynik, odchylenie standardowe i odsetek sciezek bez kolizji."""
    v = np.array([r["best_value"] for r in results], dtype=float)
    c = np.array([r["best_collision"] for r in results], dtype=float)
    t = np.array([r["time_s"] for r in results], dtype=float)
    return {
        "runs": len(v),
        "best": float(v.min()),
        "mean": float(v.mean()),
        "worst": float(v.max()),
        "std": float(v.std(ddof=1)) if len(v) > 1 else 0.0,
        "collision_free": float(np.mean(c == 0.0)),
        "mean_time_s": float(t.mean()),
    }
