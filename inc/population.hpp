#pragma once
#include <random>
#include <vector>
#include "point.h"
#include "map.hpp"
#include "goal_function.hpp"

// Cykl ewolucyjny AG:
//   populacja --SELEKCJA (ruletka)--> pula rodzicielska
//   --OPERACJE GENETYCZNE (krzyzowanie + mutacja)--> potomkowie
//   --funkcja celu--> stopien przystosowania --PODSTAWIENIE--> nowa populacja
//
// Osobnik = wektor PUNKTOW POSREDNICH sciezki. Start i cel (robot_path)
// sa wspolne dla wszystkich i dokleja sie je przy liczeniu funkcji celu.

// Parametry algorytmu (wartosci domyslne = dotychczasowe stale).
struct Parameters {
    int pop_size = 100;               // liczba osobnikow
    double crossover_prob = 0.8;      // prawdopodobienstwo krzyzowania pary rodzicow
    double mutation_prob = 0.1;       // prawdopodobienstwo mutacji pojedynczego genu (punktu)
    double mutation_sigma = 2.0;      // odchylenie przesuniecia punktu [komorki mapy]
    double insert_prob = 0.05;        // prawdopodobienstwo dodania punktu do osobnika
    double delete_prob = 0.05;        // prawdopodobienstwo usuniecia punktu z osobnika
    int elite = 2;                    // ilu najlepszych przechodzi bez zmian
    double collision_penalty = COLLISION_PENALTY;   // kara w funkcji celu
};

class Population {
private:
    int max_path;                                   // maks. liczba punktow posrednich
    std::vector<Point> robot_path;                  // {start, cel}
    std::vector<std::vector<Point>> population;     // osobnicy
    std::vector<double> goal_values;                // wartosc funkcji celu (mniej = lepiej)
    std::vector<double> fitness;                    // stopien przystosowania (wiecej = lepiej)
    const Map &map;
    Parameters params;
    std::mt19937 gen;

    std::vector<Point> full_path(const std::vector<Point> &individual) const;
    void evaluate(const std::vector<std::vector<Point>> &group,
                  std::vector<double> &values, std::vector<double> &fit);
    void crossover(const std::vector<Point> &parent1, const std::vector<Point> &parent2,
                   std::vector<Point> &child1, std::vector<Point> &child2);
    void mutation(std::vector<Point> &individual);
    void clamp_to_map(Point &pt) const;
public:
    Population(int, std::vector<Point> &, const Map &, unsigned seed,
               const Parameters &params = Parameters());
    int random_gen(int);                            // losowa liczba calkowita z [0, max]
    double random_real(double, double);             // losowa liczba rzeczywista z [min, max)

    std::vector<std::vector<Point>> selection();    // ruletka -> pula rodzicielska
    std::vector<std::vector<Point>> genetic_operations(const std::vector<std::vector<Point>> &parents);
    void replacement(std::vector<std::vector<Point>> &offspring);   // podstawienie
    void next_generation();                         // jeden pelny cykl

    std::vector<Point> best_path() const;           // pelna sciezka najlepszego osobnika
    double best_value() const;
    double mean_value() const;
    double best_collision() const;

    // dostep tylko do odczytu (np. do wykresow w Pythonie)
    const std::vector<std::vector<Point>> &get_population() const;
    const std::vector<double> &get_goal_values() const;
    const std::vector<double> &get_fitness() const;
    const Parameters &get_parameters() const;
};
