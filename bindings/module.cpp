// Binding pybind11: klasy z inc/ widoczne w Pythonie jako modul robot_ga._core.
// Tylko tlumaczenie typow - cala logika algorytmu jest w src/.
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

#include "goal_function.hpp"
#include "map.hpp"
#include "point.h"
#include "population.hpp"

namespace py = pybind11;

namespace {

// wektor punktow -> tablica numpy (n, 2)
py::array_t<double> to_array(const std::vector<Point> &path) {
    py::array_t<double> out({(py::ssize_t)path.size(), (py::ssize_t)2});
    auto a = out.mutable_unchecked<2>();
    for (py::ssize_t k = 0; k < (py::ssize_t)path.size(); k++) {
        a(k, 0) = path[k].xp;
        a(k, 1) = path[k].yp;
    }
    return out;
}

// tablica (n, 2) albo lista par -> wektor punktow
std::vector<Point> to_points(const py::array_t<double, py::array::c_style | py::array::forcecast> &arr) {
    if (arr.ndim() != 2 || arr.shape(1) != 2)
        throw std::invalid_argument("oczekiwano tablicy o ksztalcie (n, 2)");
    auto a = arr.unchecked<2>();
    std::vector<Point> out;
    for (py::ssize_t k = 0; k < arr.shape(0); k++) out.emplace_back(a(k, 0), a(k, 1));
    return out;
}

// Population trzyma referencje do mapy - ten wrapper trzyma kopie mapy
// i populacje razem, zeby mapa nie zniknela, gdy Python ja zwolni.
struct PyPopulation {
    std::unique_ptr<Map> map;
    std::unique_ptr<Population> pop;
    int generation = 0;

    PyPopulation(const Map &m, int max_path, unsigned seed, const Parameters &params)
        : map(std::make_unique<Map>(m)) {
        std::vector<Point> robot_path = {map->get_start(), map->get_goal()};
        pop = std::make_unique<Population>(max_path, robot_path, *map, seed, params);
    }
};

} // namespace

PYBIND11_MODULE(_core, m) {
    m.doc() = "Algorytm ewolucyjny do planowania sciezki robota mobilnego (rdzen C++)";

    py::class_<Point>(m, "Point")
        .def(py::init<>())
        .def(py::init<double, double>(), py::arg("x"), py::arg("y"))
        .def_readwrite("xp", &Point::xp)
        .def_readwrite("yp", &Point::yp)
        .def("__repr__", [](const Point &p) {
            return "Point(" + std::to_string(p.xp) + ", " + std::to_string(p.yp) + ")";
        });

    py::class_<Map>(m, "Map")
        .def(py::init<>())
        .def(py::init([](const std::string &file) { Map mp; mp.load(file); return mp; }),
             py::arg("file_name"), "Wczytuje mape z pliku")
        .def("load", &Map::load, py::arg("file_name"))
        .def_property_readonly("width", &Map::get_width)
        .def_property_readonly("height", &Map::get_height)
        .def_property_readonly("start", [](const Map &mp) { return py::make_tuple(mp.get_start().xp, mp.get_start().yp); })
        .def_property_readonly("goal", [](const Map &mp) { return py::make_tuple(mp.get_goal().xp, mp.get_goal().yp); })
        .def_property_readonly("grid", [](const Map &mp) {
            // (wysokosc, szerokosc), True = przeszkoda
            py::array_t<bool> out({(py::ssize_t)mp.get_height(), (py::ssize_t)mp.get_width()});
            auto a = out.mutable_unchecked<2>();
            const auto &g = mp.get_grid();
            for (int y = 0; y < mp.get_height(); y++)
                for (int x = 0; x < mp.get_width(); x++) a(y, x) = g[y * mp.get_width() + x] != 0;
            return out;
        })
        .def("is_free", &Map::is_free, py::arg("x"), py::arg("y"))
        .def("collision_length", [](const Map &mp, double ax, double ay, double bx, double by) {
            return mp.collision_length(Point(ax, ay), Point(bx, by));
        }, py::arg("ax"), py::arg("ay"), py::arg("bx"), py::arg("by"));

    py::class_<Parameters>(m, "Parameters")
        .def(py::init<>())
        .def_readwrite("pop_size", &Parameters::pop_size)
        .def_readwrite("crossover_prob", &Parameters::crossover_prob)
        .def_readwrite("mutation_prob", &Parameters::mutation_prob)
        .def_readwrite("mutation_sigma", &Parameters::mutation_sigma)
        .def_readwrite("insert_prob", &Parameters::insert_prob)
        .def_readwrite("delete_prob", &Parameters::delete_prob)
        .def_readwrite("elite", &Parameters::elite)
        .def_readwrite("collision_penalty", &Parameters::collision_penalty)
        .def("__repr__", [](const Parameters &p) {
            return "Parameters(pop_size=" + std::to_string(p.pop_size) +
                   ", crossover_prob=" + std::to_string(p.crossover_prob) +
                   ", mutation_prob=" + std::to_string(p.mutation_prob) +
                   ", mutation_sigma=" + std::to_string(p.mutation_sigma) +
                   ", insert_prob=" + std::to_string(p.insert_prob) +
                   ", delete_prob=" + std::to_string(p.delete_prob) +
                   ", elite=" + std::to_string(p.elite) +
                   ", collision_penalty=" + std::to_string(p.collision_penalty) + ")";
        });

    // Funkcja celu dla dowolnej sciezki (pelnej: start ... cel)
    m.def("goal_value", [](const py::array_t<double, py::array::c_style | py::array::forcecast> &path,
                           const Map &mp, double penalty) {
        std::vector<Point> pts = to_points(path);
        Goal_function goal(pts, mp, penalty);
        return py::make_tuple(goal.value(), goal.length(), goal.collision());
    }, py::arg("path"), py::arg("map"), py::arg("penalty") = COLLISION_PENALTY,
       "Zwraca (wartosc, dlugosc, kolizje) dla pelnej sciezki (n, 2)");

    py::class_<PyPopulation>(m, "Population")
        .def(py::init<const Map &, int, unsigned, const Parameters &>(),
             py::arg("map"), py::arg("max_path") = 8, py::arg("seed") = 1,
             py::arg("params") = Parameters())
        .def("next_generation", [](PyPopulation &p) {
            {
                py::gil_scoped_release release;
                p.pop->next_generation();
            }
            p.generation++;
        }, "Jeden cykl: selekcja -> operacje genetyczne -> podstawienie")
        .def("run", [](PyPopulation &p, int generations) {
            py::gil_scoped_release release;
            for (int g = 0; g < generations; g++) p.pop->next_generation();
            p.generation += generations;
        }, py::arg("generations"), "Wykonuje kilka pokolen bez zbierania historii")
        .def_property_readonly("generation", [](const PyPopulation &p) { return p.generation; })
        .def_property_readonly("map", [](const PyPopulation &p) { return *p.map; })
        .def_property_readonly("params", [](const PyPopulation &p) { return p.pop->get_parameters(); })
        .def("best_path", [](const PyPopulation &p) { return to_array(p.pop->best_path()); },
             "Pelna najlepsza sciezka (start ... cel) jako tablica (n, 2)")
        .def("best_value", [](const PyPopulation &p) { return p.pop->best_value(); })
        .def("mean_value", [](const PyPopulation &p) { return p.pop->mean_value(); })
        .def("best_collision", [](const PyPopulation &p) { return p.pop->best_collision(); })
        .def("individuals", [](const PyPopulation &p) {
            py::list out;
            for (const auto &ind : p.pop->get_population()) out.append(to_array(ind));
            return out;
        }, "Punkty posrednie kazdego osobnika: lista tablic (n_i, 2)")
        .def("goal_values", [](const PyPopulation &p) {
            return py::array_t<double>(p.pop->get_goal_values().size(), p.pop->get_goal_values().data());
        })
        .def("fitness", [](const PyPopulation &p) {
            return py::array_t<double>(p.pop->get_fitness().size(), p.pop->get_fitness().data());
        });
}
