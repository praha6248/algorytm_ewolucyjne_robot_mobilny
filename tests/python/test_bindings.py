"""Testy bindingu: python -m pytest tests/python (z katalogu projektu)."""

import numpy as np

import evopath


def test_map_loads():
    m = evopath.Map.load("maps/simple.txt")
    assert m.grid.shape == (m.height, m.width)
    assert m.is_free(*m.start)


def test_solver_runs_and_returns_arrays():
    m = evopath.Map.load("maps/simple.txt")
    cfg = evopath.make_config({"population_size": 20, "snapshot_every": 5})
    s = evopath.Solver(m, cfg)
    s.run(10)
    assert s.generation == 10
    path = s.best_path()
    assert path.shape[1] == 2
    np.testing.assert_allclose(path[0], m.start)
    np.testing.assert_allclose(path[-1], m.goal)
    assert len(s.history()["best"]) == 11
    snap = s.snapshots()[0]
    assert len(snap["paths"]) == 20
    assert all(p.ndim == 2 and p.shape[1] == 2 for p in snap["paths"])
    pop = s.population()
    assert len(pop["paths"]) == 20 and pop["fitness"].shape == (20,)
