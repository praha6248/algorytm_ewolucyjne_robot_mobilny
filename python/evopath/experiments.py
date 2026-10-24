"""Uruchamianie eksperymentow: konfiguracja z dict/JSON, serie z roznymi ziarnami."""

from __future__ import annotations

import json
import time
from pathlib import Path

import numpy as np

from ._core import Backend, GAConfig, Map, Solver

_BACKENDS = {"cpu": Backend.CPU, "openmp": Backend.OpenMP}


def make_config(params: dict | None = None, **overrides) -> GAConfig:
    """Tworzy GAConfig ze slownika (np. wczytanego z configs/*.json).

    Klucz "weights" to slownik wag, "backend" to "cpu" / "openmp".
    """
    params = {**(params or {}), **overrides}
    cfg = GAConfig()
    for key, value in params.items():
        if key == "weights":
            for wk, wv in value.items():
                if not hasattr(cfg.weights, wk):
                    raise KeyError(f"Nieznana waga: {wk}")
                w = cfg.weights
                setattr(w, wk, wv)
                cfg.weights = w
        elif key == "backend":
            cfg.backend = _BACKENDS[value] if isinstance(value, str) else value
        elif key in ("map", "generations", "seeds", "name"):
            continue  # klucze eksperymentu, nie GA
        elif hasattr(cfg, key):
            setattr(cfg, key, value)
        else:
            raise KeyError(f"Nieznany parametr: {key}")
    return cfg


def load_json(path: str | Path) -> dict:
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def run_once(map_obj: Map, cfg: GAConfig, generations: int) -> dict:
    """Jedno uruchomienie GA; zwraca slownik z wynikiem i historia."""
    solver = Solver(map_obj, cfg)
    t0 = time.perf_counter()
    solver.run(generations)
    elapsed = time.perf_counter() - t0
    return {
        "seed": cfg.seed,
        "best_fitness": solver.best_fitness,
        "best_path": solver.best_path(),
        "history": solver.history(),
        "snapshots": solver.snapshots(),
        "time_s": elapsed,
        "backend": solver.backend,
    }


def run_series(map_obj: Map, params: dict, generations: int, seeds) -> list[dict]:
    """Seria niezaleznych uruchomien (np. seeds=range(30))."""
    results = []
    for seed in seeds:
        cfg = make_config(params, seed=int(seed))
        results.append(run_once(map_obj, cfg, generations))
    return results


def summarize(results: list[dict]) -> dict:
    """Najlepszy, sredni, najgorszy wynik i odchylenie standardowe serii."""
    f = np.array([r["best_fitness"] for r in results], dtype=float)
    t = np.array([r["time_s"] for r in results], dtype=float)
    return {
        "runs": len(f),
        "best": float(f.min()),
        "mean": float(f.mean()),
        "worst": float(f.max()),
        "std": float(f.std(ddof=1)) if len(f) > 1 else 0.0,
        "mean_time_s": float(t.mean()),
    }
