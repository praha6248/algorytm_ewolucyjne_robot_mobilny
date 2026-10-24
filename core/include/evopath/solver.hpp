#pragma once
#include <memory>
#include <vector>
#include "evopath/config.hpp"
#include "evopath/evaluator.hpp"
#include "evopath/history.hpp"
#include "evopath/map.hpp"
#include "evopath/population.hpp"
#include "evopath/rng.hpp"

namespace evo {

class Solver {
public:
    Solver(Map map, GAConfig cfg);

    // Wykonuje n kolejnych pokolen (mozna wolac wielokrotnie - "partiami").
    void run(int generations);

    int generation() const { return generation_; }
    float best_fitness() const;
    // Najlepsza sciezka razem ze startem i celem.
    std::vector<Point> best_path() const;

    const Map& map() const { return map_; }
    const GAConfig& config() const { return cfg_; }
    const Population& population() const { return pop_; }
    const History& history() const { return history_; }
    const char* backend_name() const { return evaluator_->name(); }

private:
    void evaluate(Population& pop);
    // TODO(student): jedno pokolenie - selekcja, krzyzowanie, mutacja,
    // elitaryzm. Implementacja: core/src/solver.cpp
    void next_generation();

    Map map_;
    GAConfig cfg_;
    Rng rng_;
    Population pop_;
    Population offspring_;
    std::unique_ptr<Evaluator> evaluator_;
    History history_;
    int generation_ = 0;
    float sigma_ = 0.0f;
};

} // namespace evo
