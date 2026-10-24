#include "evopath/evaluator.hpp"

#include <stdexcept>

namespace evo {

bool backend_available(Backend backend) {
    switch (backend) {
        case Backend::CPU: return true;
        case Backend::OpenMP:
#ifdef EVO_HAS_OPENMP
            return true;
#else
            return false;
#endif
    }
    return false;
}

std::unique_ptr<Evaluator> make_evaluator(Backend backend) {
    switch (backend) {
        case Backend::CPU: return make_cpu_evaluator();
        case Backend::OpenMP:
#ifdef EVO_HAS_OPENMP
            return make_openmp_evaluator();
#else
            throw std::runtime_error("Backend OpenMP nie zostal wkompilowany");
#endif
    }
    throw std::runtime_error("Nieznany backend");
}

} // namespace evo
