"""evopath - algorytm ewolucyjny do planowania sciezki robota.

Rdzen obliczeniowy jest w C++ (modul evopath._core), tu sa narzedzia
do eksperymentow i wykresow.
"""

from ._core import (
    Backend,
    FitnessWeights,
    GAConfig,
    Map,
    Solver,
    __version__,
    backend_available,
)
from .experiments import make_config, run_once, run_series, summarize

__all__ = [
    "Backend",
    "FitnessWeights",
    "GAConfig",
    "Map",
    "Solver",
    "__version__",
    "backend_available",
    "make_config",
    "run_once",
    "run_series",
    "summarize",
]
