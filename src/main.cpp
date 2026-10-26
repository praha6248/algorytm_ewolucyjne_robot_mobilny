// Planowanie sciezki robota mobilnego algorytmem ewolucyjnym.
//
//   ./robot_ga [mapa=maps/map1.txt] [pokolenia=300] [max_punktow=8] [seed=1]
//
// Zapisuje results/best_path.csv (najlepsza sciezka) i results/history.csv
// (najlepsza i srednia wartosc funkcji celu w kazdym pokoleniu).
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "map.hpp"
#include "population.hpp"

int main(int argc, char **argv) {
    const std::string map_file = argc > 1 ? argv[1] : "maps/map1.txt";
    const int generations = argc > 2 ? std::atoi(argv[2]) : 300;
    const int max_points = argc > 3 ? std::atoi(argv[3]) : 8;
    const unsigned seed = argc > 4 ? (unsigned)std::strtoul(argv[4], nullptr, 10) : 1u;

    try {
        Map map;
        map.load(map_file);

        std::vector<Point> robot_path = {map.get_start(), map.get_goal()};
        Population population(max_points, robot_path, map, seed);

        std::filesystem::create_directories("results");
        std::ofstream history("results/history.csv");
        history << "generation,best,mean\n";
        history << 0 << ',' << population.best_value() << ',' << population.mean_value() << '\n';

        for (int g = 1; g <= generations; g++) {
            population.next_generation();
            history << g << ',' << population.best_value() << ',' << population.mean_value() << '\n';
            if (g % 50 == 0 || g == generations)
                std::cout << "pokolenie " << g << ": najlepszy = " << population.best_value()
                          << ", sredni = " << population.mean_value() << '\n';
        }

        std::ofstream out("results/best_path.csv");
        out << "x,y\n";
        for (const Point &pt : population.best_path()) out << pt.xp << ',' << pt.yp << '\n';

        std::cout << "Kolizje najlepszej sciezki: " << population.best_collision() << '\n'
                  << "Zapisano results/best_path.csv i results/history.csv\n";
    } catch (const std::exception &e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
