#pragma once
#include <vector>
#include "evopath/population.hpp"

namespace evo {

struct Snapshot {
    int generation = 0;
    std::vector<std::vector<Point>> paths;   // punkty posrednie kazdego osobnika
};

// Statystyki z kazdego pokolenia + opcjonalne migawki populacji.
struct History {
    std::vector<int>   generation;
    std::vector<float> best;
    std::vector<float> mean;
    std::vector<float> worst;
    std::vector<Snapshot> snapshots;

    void record(const Population& pop, int gen, bool take_snapshot);
    void clear();
};

} // namespace evo
