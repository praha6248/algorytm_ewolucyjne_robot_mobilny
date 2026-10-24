#pragma once
#include <cstddef>
#include <vector>

#include "evopath/map.hpp"   // struct Point { float x, y; }

namespace evo {

// Jeden osobnik = jedna sciezka.
// `points` to punkty POSREDNIE - start i cel nie sa zapisane w chromosomie,
// dokleja sie je przy ocenie. Pusta lista = prosta start -> cel.
//
//   start -> points[0] -> points[1] -> ... -> points.back() -> cel
struct Individual {
    std::vector<Point> points;
    float fitness = 0.0f;   // mniej = lepiej
};

// Populacja = wektor osobnikow.
using Population = std::vector<Individual>;

// Indeks najlepszego osobnika (najmniejszy fitness). Pusta populacja -> -1.
inline int best_index(const Population& pop) {
    if (pop.empty()) return -1;
    int b = 0;
    for (int i = 1; i < static_cast<int>(pop.size()); ++i)
        if (pop[i].fitness < pop[b].fitness) b = i;
    return b;
}

// Pelna sciezka osobnika: start + punkty posrednie + cel.
inline std::vector<Point> full_path(const Individual& ind, Point start, Point goal) {
    std::vector<Point> path;
    path.reserve(ind.points.size() + 2);
    path.push_back(start);
    path.insert(path.end(), ind.points.begin(), ind.points.end());
    path.push_back(goal);
    return path;
}

} // namespace evo
