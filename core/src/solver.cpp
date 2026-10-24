#include "evopath/solver.hpp"

#include <stdexcept>
#include <utility>

#include "evopath/operators.hpp"

namespace evo {

Solver::Solver(Map map, GAConfig cfg)
    : map_(std::move(map)), cfg_(cfg), rng_(cfg.seed),
      evaluator_(make_evaluator(cfg.backend)), sigma_(cfg.mutation_sigma) {
    if (cfg_.population_size < 2) throw std::invalid_argument("population_size < 2");
    if (cfg_.max_points < 1) throw std::invalid_argument("max_points < 1");
    if (cfg_.initial_points > cfg_.max_points) throw std::invalid_argument("initial_points > max_points");
    if (cfg_.elite < 0 || cfg_.elite > cfg_.population_size) throw std::invalid_argument("zly elite");

    pop_.resize(cfg_.population_size);
    offspring_.resize(cfg_.population_size);

    ops::initialize(pop_, map_, cfg_, rng_);
    evaluate(pop_);
    history_.record(pop_, 0, cfg_.snapshot_every > 0);
}

void Solver::evaluate(Population& pop) {
    evaluator_->evaluate(map_, cfg_, pop);
}

void Solver::run(int generations) {
    for (int g = 0; g < generations; ++g) {
        next_generation();
        ++generation_;
        sigma_ *= cfg_.sigma_decay;
        const bool snap = cfg_.snapshot_every > 0 && generation_ % cfg_.snapshot_every == 0;
        history_.record(pop_, generation_, snap);
    }
}

void Solver::next_generation() {
    // TODO(student): jedno pokolenie, np.:
    //   1. elitaryzm: cfg_.elite najlepszych z pop_ -> offspring_
    //      (np. posortuj kopie indeksow wg pop_[i].fitness)
    //   2. dopoki offspring_ nie jest pelna (offspring_.size() == pop_.size()):
    //        a = ops::tournament_select(pop_, cfg_.tournament_size, rng_), b = ...
    //        ops::crossover(pop_[a], pop_[b], c1, c2, cfg_, rng_)   (z prawdop. cfg_.crossover_prob)
    //        ops::mutate(c1, map_, cfg_, sigma_, rng_), to samo dla c2
    //   3. evaluate(offspring_)
    //   4. std::swap(pop_, offspring_)
}

float Solver::best_fitness() const {
    return pop_[best_index(pop_)].fitness;
}

std::vector<Point> Solver::best_path() const {
    return full_path(pop_[best_index(pop_)], map_.start(), map_.goal());
}

} // namespace evo
