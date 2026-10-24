#include "evopath/evaluator.hpp"

namespace evo {
namespace {

// TODO(student): przystosowanie jednego osobnika. Pelna sciezka to
// start -> ind.points... -> cel (pomocniczo: full_path(ind, map.start(), map.goal())).
// Skladniki: dlugosc, map.collision_length(...), opcjonalnie gladkosc i odstep
// od przeszkod; wagi w cfg.weights. Mniej = lepiej.
float evaluate_one(const Map& map, const GAConfig& cfg, const Individual& ind) {
    (void)map; (void)cfg; (void)ind;
    return 0.0f;
}

class CpuEvaluator final : public Evaluator {
public:
    void evaluate(const Map& map, const GAConfig& cfg, Population& pop) override {
        for (Individual& ind : pop) ind.fitness = evaluate_one(map, cfg, ind);
    }
    const char* name() const override { return "cpu"; }
};

} // namespace

// Udostepnione, zeby backend OpenMP liczyl dokladnie to samo co CPU.
float evaluate_individual(const Map& map, const GAConfig& cfg, const Individual& ind) {
    return evaluate_one(map, cfg, ind);
}

std::unique_ptr<Evaluator> make_cpu_evaluator() { return std::make_unique<CpuEvaluator>(); }

} // namespace evo
