#include <omp.h>

#include "evopath/evaluator.hpp"

namespace evo {

// Zdefiniowane w backends/cpu/cpu_evaluator.cpp
float evaluate_individual(const Map& map, const GAConfig& cfg, const Individual& ind);

namespace {

class OpenMpEvaluator final : public Evaluator {
public:
    void evaluate(const Map& map, const GAConfig& cfg, Population& pop) override {
        // Gotowe: ta sama ocena co na CPU (evaluate_one), rownolegle po osobnikach.
        const int n = static_cast<int>(pop.size());
#pragma omp parallel for schedule(dynamic, 8)
        for (int i = 0; i < n; ++i) pop[i].fitness = evaluate_individual(map, cfg, pop[i]);
    }
    const char* name() const override { return "openmp"; }
};

} // namespace

std::unique_ptr<Evaluator> make_openmp_evaluator() { return std::make_unique<OpenMpEvaluator>(); }

} // namespace evo
