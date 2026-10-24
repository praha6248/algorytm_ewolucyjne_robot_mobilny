#pragma once
#include "evopath/config.hpp"
#include "evopath/map.hpp"
#include "evopath/population.hpp"
#include "evopath/rng.hpp"

// Operatory genetyczne. Wszystkie do zaimplementowania w core/src/operators.cpp.
namespace evo::ops {

// TODO(student): wypelnij populacje losowymi sciezkami. Populacja ma juz
// rozmiar cfg.population_size - ustaw pop[i].points dla kazdego osobnika
// (np. cfg.initial_points punktow na prostej start->cel z szumem).
void initialize(Population& pop, const Map& map, const GAConfig& cfg, Rng& rng);

// TODO(student): zwroc indeks wybranego rodzica (turniej rozmiaru k).
int tournament_select(const Population& pop, int k, Rng& rng);

// TODO(student): z rodzicow a i b utworz dzieci c1 i c2.
// Dzieci moga miec inna liczbe punktow niz rodzice, ale najwyzej cfg.max_points.
void crossover(const Individual& a, const Individual& b,
               Individual& c1, Individual& c2, const GAConfig& cfg, Rng& rng);

// TODO(student): pojedyncze mutacje osobnika.
void mutate_move(Individual& ind, const Map& map, float sigma, Rng& rng);
void mutate_insert(Individual& ind, const Map& map, const GAConfig& cfg, Rng& rng);
void mutate_delete(Individual& ind, Rng& rng);
void mutate_smooth(Individual& ind, Rng& rng);

// TODO(student): wybierz i zastosuj mutacje (prawdopodobienstwa z cfg).
void mutate(Individual& ind, const Map& map, const GAConfig& cfg, float sigma, Rng& rng);

} // namespace evo::ops
