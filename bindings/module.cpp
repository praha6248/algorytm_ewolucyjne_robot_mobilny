// Binding pybind11 - tylko tlumaczenie typow, bez logiki algorytmu.
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstring>

#include "evopath/config.hpp"
#include "evopath/evaluator.hpp"
#include "evopath/map.hpp"
#include "evopath/solver.hpp"

namespace py = pybind11;
using namespace evo;

namespace {

py::array_t<float> path_to_array(const std::vector<Point>& path) {
    py::array_t<float> out({static_cast<py::ssize_t>(path.size()), py::ssize_t{2}});
    auto a = out.mutable_unchecked<2>();
    for (py::ssize_t k = 0; k < static_cast<py::ssize_t>(path.size()); ++k) {
        a(k, 0) = path[k].x;
        a(k, 1) = path[k].y;
    }
    return out;
}

template <typename T>
py::array_t<T> to_array(const std::vector<T>& v) {
    py::array_t<T> out(static_cast<py::ssize_t>(v.size()));
    if (!v.empty()) std::memcpy(out.mutable_data(), v.data(), v.size() * sizeof(T));
    return out;
}

// Lista sciezek (punkty posrednie) -> lista tablic numpy (n_i, 2)
py::list paths_to_list(const std::vector<std::vector<Point>>& paths) {
    py::list out;
    for (const auto& p : paths) out.append(path_to_array(p));
    return out;
}

} // namespace

PYBIND11_MODULE(_core, m) {
    m.doc() = "evopath - algorytm ewolucyjny do planowania sciezki robota (rdzen C++)";
    m.attr("__version__") = EVO_VERSION;

    py::enum_<Backend>(m, "Backend")
        .value("CPU", Backend::CPU)
        .value("OpenMP", Backend::OpenMP);

    m.def("backend_available", &backend_available, py::arg("backend"));

    py::class_<FitnessWeights>(m, "FitnessWeights")
        .def(py::init<>())
        .def_readwrite("length", &FitnessWeights::length)
        .def_readwrite("collision", &FitnessWeights::collision)
        .def_readwrite("smoothness", &FitnessWeights::smoothness)
        .def_readwrite("clearance", &FitnessWeights::clearance);

    py::class_<GAConfig>(m, "GAConfig")
        .def(py::init<>())
        .def_readwrite("population_size", &GAConfig::population_size)
        .def_readwrite("max_points", &GAConfig::max_points)
        .def_readwrite("initial_points", &GAConfig::initial_points)
        .def_readwrite("elite", &GAConfig::elite)
        .def_readwrite("tournament_size", &GAConfig::tournament_size)
        .def_readwrite("crossover_prob", &GAConfig::crossover_prob)
        .def_readwrite("mutation_prob", &GAConfig::mutation_prob)
        .def_readwrite("mutation_sigma", &GAConfig::mutation_sigma)
        .def_readwrite("sigma_decay", &GAConfig::sigma_decay)
        .def_readwrite("seed", &GAConfig::seed)
        .def_readwrite("backend", &GAConfig::backend)
        .def_readwrite("snapshot_every", &GAConfig::snapshot_every)
        .def_readwrite("weights", &GAConfig::weights);

    py::class_<Map>(m, "Map")
        .def_static("load", &Map::load, py::arg("path"))
        .def_static("from_string", &Map::from_string, py::arg("text"))
        .def_property_readonly("width", &Map::width)
        .def_property_readonly("height", &Map::height)
        .def_property_readonly("start", [](const Map& mp) { return py::make_tuple(mp.start().x, mp.start().y); })
        .def_property_readonly("goal", [](const Map& mp) { return py::make_tuple(mp.goal().x, mp.goal().y); })
        .def_property_readonly("grid", [](const Map& mp) {
            py::array_t<std::uint8_t> out({static_cast<py::ssize_t>(mp.height()),
                                           static_cast<py::ssize_t>(mp.width())});
            std::memcpy(out.mutable_data(), mp.grid().data(), mp.grid().size());
            return out;
        })
        .def("is_free", &Map::is_free, py::arg("x"), py::arg("y"))
        .def("collision_length", [](const Map& mp, float ax, float ay, float bx, float by) {
            return mp.collision_length({ax, ay}, {bx, by});
        }, py::arg("ax"), py::arg("ay"), py::arg("bx"), py::arg("by"));

    py::class_<Solver>(m, "Solver")
        .def(py::init<Map, GAConfig>(), py::arg("map"), py::arg("config"))
        // GIL zwolniony na czas obliczen - Jupyter nie zamarza.
        .def("run", &Solver::run, py::arg("generations"), py::call_guard<py::gil_scoped_release>())
        .def_property_readonly("generation", &Solver::generation)
        .def_property_readonly("best_fitness", &Solver::best_fitness)
        .def_property_readonly("backend", &Solver::backend_name)
        .def("best_path", [](const Solver& s) { return path_to_array(s.best_path()); })
        .def("history", [](const Solver& s) {
            const History& h = s.history();
            py::dict d;
            d["generation"] = to_array(h.generation);
            d["best"] = to_array(h.best);
            d["mean"] = to_array(h.mean);
            d["worst"] = to_array(h.worst);
            return d;
        })
        // Migawki: lista {generation, paths}; paths = lista tablic (n_i, 2)
        // z punktami posrednimi kazdego osobnika (bez startu i celu).
        .def("snapshots", [](const Solver& s) {
            py::list out;
            for (const Snapshot& sn : s.history().snapshots) {
                py::dict d;
                d["generation"] = sn.generation;
                d["paths"] = paths_to_list(sn.paths);
                out.append(d);
            }
            return out;
        })
        // Aktualna populacja: {paths, fitness}
        .def("population", [](const Solver& s) {
            const Population& p = s.population();
            std::vector<std::vector<Point>> paths;
            std::vector<float> fitness;
            paths.reserve(p.size());
            fitness.reserve(p.size());
            for (const Individual& ind : p) {
                paths.push_back(ind.points);
                fitness.push_back(ind.fitness);
            }
            py::dict d;
            d["paths"] = paths_to_list(paths);
            d["fitness"] = to_array(fitness);
            return d;
        });
}
