#pragma once
#include <memory>
#include "evopath/config.hpp"
#include "evopath/map.hpp"
#include "evopath/population.hpp"

namespace evo {

// Wspolny interfejs oceny przystosowania. Kazdy backend wypelnia
// pop.fitness[i] dla wszystkich osobnikow.
class Evaluator {
public:
    virtual ~Evaluator() = default;
    virtual void evaluate(const Map& map, const GAConfig& cfg, Population& pop) = 0;
    virtual const char* name() const = 0;
};

// Fabryka: wybiera backend wg konfiguracji. Rzuca std::runtime_error,
// gdy backend nie zostal wkompilowany. Implementacja: core/src/evaluator.cpp
std::unique_ptr<Evaluator> make_evaluator(Backend backend);

bool backend_available(Backend backend);

// Fabryki poszczegolnych backendow (backends/<nazwa>/)
std::unique_ptr<Evaluator> make_cpu_evaluator();
#ifdef EVO_HAS_OPENMP
std::unique_ptr<Evaluator> make_openmp_evaluator();
#endif

} // namespace evo
