#pragma once
#include <vector>
#include <cmath>
#include "point.h"
#include "map.hpp"

// domyslna kara za kazda jednostke dlugosci w przeszkodzie
const double COLLISION_PENALTY = 100.0;

// Funkcja celu (minimalizowana):
//   value = dlugosc sciezki + penalty * dlugosc fragmentow w przeszkodach
// robot_path to PELNA sciezka: start, punkty posrednie, cel.
class Goal_function {
private:
    std::vector<Point> robot_path;
    const Map *map;
    double penalty;
public:
    Goal_function(std::vector<Point> &path);                  // bez mapy: tylko dlugosc
    Goal_function(std::vector<Point> &path, const Map &map,
                  double penalty = COLLISION_PENALTY);
    double length();
    double collision();
    double value();
};
