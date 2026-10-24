// Program do uruchamiania GA bez Pythona:
//   ./run_ga maps/simple.txt [pokolenia=500] [backend=cpu|openmp] [seed=42]
// Zapisuje results/best_path.csv oraz results/history.csv.
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "evopath/solver.hpp"

using namespace evo;

static Backend parse_backend(const std::string& s) {
    if (s == "cpu") return Backend::CPU;
    if (s == "openmp" || s == "omp") return Backend::OpenMP;
    throw std::runtime_error("Nieznany backend: " + s);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uzycie: " << argv[0] << " <mapa> [pokolenia] [cpu|openmp] [seed]\n";
        return 1;
    }
    try {
        const std::string map_path = argv[1];
        const int generations = argc > 2 ? std::atoi(argv[2]) : 500;

        GAConfig cfg;
        if (argc > 3) cfg.backend = parse_backend(argv[3]);
        if (argc > 4) cfg.seed = std::strtoull(argv[4], nullptr, 10);

        Solver solver(Map::load(map_path), cfg);

        const auto t0 = std::chrono::steady_clock::now();
        solver.run(generations);
        const auto t1 = std::chrono::steady_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

        std::printf("backend=%s pokolenia=%d czas=%.1f ms najlepszy=%.4f\n",
                    solver.backend_name(), solver.generation(), ms, solver.best_fitness());

        std::filesystem::create_directories("results");
        std::ofstream path_csv("results/best_path.csv");
        path_csv << "x,y\n";
        for (const Point& p : solver.best_path()) path_csv << p.x << ',' << p.y << '\n';

        const History& h = solver.history();
        std::ofstream hist_csv("results/history.csv");
        hist_csv << "generation,best,mean,worst\n";
        for (std::size_t g = 0; g < h.best.size(); ++g)
            hist_csv << h.generation[g] << ',' << h.best[g] << ',' << h.mean[g] << ',' << h.worst[g] << '\n';

        std::printf("Zapisano results/best_path.csv i results/history.csv\n");
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
