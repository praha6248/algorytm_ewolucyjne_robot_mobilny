"""robot_ga - algorytm ewolucyjny do planowania sciezki robota mobilnego.

Rdzen obliczeniowy jest w C++ (modul robot_ga._core), tu sa narzedzia
do uruchamiania eksperymentow. Wykresy: robot_ga.plots.

    import robot_ga
    m = robot_ga.Map("maps/map1.txt")
    result = robot_ga.run(m, generations=300, seed=1)
    print(result["best_value"])
"""

from ._core import Map, Parameters, Point, Population, goal_value
from .experiments import make_parameters, run, run_series, summarize

__all__ = [
    "Map",
    "Parameters",
    "Point",
    "Population",
    "goal_value",
    "make_parameters",
    "run",
    "run_series",
    "summarize",
]
