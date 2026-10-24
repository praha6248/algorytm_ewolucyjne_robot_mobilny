#include "evopath/operators.hpp"

// Tu implementujesz operatory genetyczne. Kazda funkcja jest na razie pusta:
// projekt sie kompiluje, ale algorytm nic nie robi.
//
// Przydatne:
//   ind.points[k].x, ind.points[k].y       - punkt k
//   ind.points.size()                      - liczba punktow
//   ind.points.push_back({x, y})           - dodaj na koniec
//   ind.points.insert(ind.points.begin() + k, {x, y})   - wstaw przed k
//   ind.points.erase(ind.points.begin() + k)            - usun punkt k
//   rng.uniform(a, b), rng.normal(0, sigma), rng.randint(a, b), rng.chance(p)

namespace evo::ops {

void initialize(Population& pop, const Map& map, const GAConfig& cfg, Rng& rng) {
    // TODO(student): dla kazdego osobnika ind w pop:
    //   ind.points.clear(); potem dodaj cfg.initial_points punktow.
    (void)pop; (void)map; (void)cfg; (void)rng;
}

int tournament_select(const Population& pop, int k, Rng& rng) {
    // TODO(student): losuj k osobnikow, zwroc indeks najlepszego
    // (najmniejszy pop[i].fitness).
    (void)pop; (void)k; (void)rng;
    return 0;
}

void crossover(const Individual& a, const Individual& b,
               Individual& c1, Individual& c2, const GAConfig& cfg, Rng& rng) {
    // TODO(student): krzyzowanie. Na razie dzieci = kopie rodzicow.
    (void)cfg; (void)rng;
    c1 = a;
    c2 = b;
}

void mutate_move(Individual& ind, const Map& map, float sigma, Rng& rng) {
    // TODO(student): przesun losowy punkt o szum gaussowski (sigma).
    (void)ind; (void)map; (void)sigma; (void)rng;
}

void mutate_insert(Individual& ind, const Map& map, const GAConfig& cfg, Rng& rng) {
    // TODO(student): wstaw nowy punkt (nie wiecej niz cfg.max_points).
    (void)ind; (void)map; (void)cfg; (void)rng;
}

void mutate_delete(Individual& ind, Rng& rng) {
    // TODO(student): usun losowy punkt (pusta lista = prosta start->cel).
    (void)ind; (void)rng;
}

void mutate_smooth(Individual& ind, Rng& rng) {
    // TODO(student): wygladz fragment, np. punkt -> srodek sasiadow.
    (void)ind; (void)rng;
}

void mutate(Individual& ind, const Map& map, const GAConfig& cfg, float sigma, Rng& rng) {
    // TODO(student): z prawdopodobienstwem cfg.mutation_prob wybierz
    // i wywolaj jedna (lub kilka) z mutacji powyzej.
    (void)ind; (void)map; (void)cfg; (void)sigma; (void)rng;
}

} // namespace evo::ops
