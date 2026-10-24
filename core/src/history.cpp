#include "evopath/history.hpp"

#include <algorithm>

namespace evo {

void History::record(const Population& pop, int gen, bool take_snapshot) {
    if (pop.empty()) return;
    float mn = pop[0].fitness, mx = pop[0].fitness;
    double sum = 0.0;
    for (const Individual& ind : pop) {
        mn = std::min(mn, ind.fitness);
        mx = std::max(mx, ind.fitness);
        sum += ind.fitness;
    }
    generation.push_back(gen);
    best.push_back(mn);
    worst.push_back(mx);
    mean.push_back(static_cast<float>(sum / pop.size()));

    if (take_snapshot) {
        Snapshot s;
        s.generation = gen;
        s.paths.reserve(pop.size());
        for (const Individual& ind : pop) s.paths.push_back(ind.points);
        snapshots.push_back(std::move(s));
    }
}

void History::clear() {
    generation.clear();
    best.clear();
    mean.clear();
    worst.clear();
    snapshots.clear();
}

} // namespace evo
