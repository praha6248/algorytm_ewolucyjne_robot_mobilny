#include "population.hpp"

#include <algorithm>
#include <numeric>

// Parametry algorytmu sa w strukturze Parameters (inc/population.hpp).

Population::Population(int max_point, std::vector<Point> &path, const Map &m, unsigned seed,
                       const Parameters &p)
: max_path(max_point), robot_path(path), map(m), params(p), gen(seed){

    // populacja poczatkowa: losowa liczba losowych punktow na mapie
    for (int i = 0; i < params.pop_size; i++){
        std::vector<Point> individual;
        int path_size = random_gen(max_path);
        for (int j = 0; j < path_size; j++){
            Point pt;
            pt.xp = random_real(0, map.get_width());
            pt.yp = random_real(0, map.get_height());
            individual.push_back(pt);
        }
        population.push_back(individual);
    }
    evaluate(population, goal_values, fitness);
}

int Population::random_gen(int max){
    std::uniform_int_distribution<> dist(0, max);
    return dist(gen);
}

double Population::random_real(double min, double max){
    std::uniform_real_distribution<> dist(min, max);
    return dist(gen);
}

std::vector<Point> Population::full_path(const std::vector<Point> &individual) const {
    std::vector<Point> path;
    path.push_back(robot_path.front());                       // start
    path.insert(path.end(), individual.begin(), individual.end());
    path.push_back(robot_path.back());                        // cel
    return path;
}

void Population::evaluate(const std::vector<std::vector<Point>> &group,
                          std::vector<double> &values, std::vector<double> &fit)
// funkcja celu -> stopien przystosowania
{
    values.resize(group.size());
    fit.resize(group.size());
    for (size_t i = 0; i < group.size(); i++){
        std::vector<Point> path = full_path(group[i]);
        Goal_function goal(path, map, params.collision_penalty);
        values[i] = goal.value();
        // ruletka wymaga przystosowania >= 0, ktore rosnie dla lepszych osobnikow,
        // a funkcje celu minimalizujemy - stad odwrotnosc
        fit[i] = 1.0 / (1.0 + values[i]);
    }
}

std::vector<std::vector<Point>> Population::selection() {
    // selekcja ruletkowa: osobnik i jest wybierany z prawdopodobienstwem
    // fitness[i] / suma(fitness)
    double total = std::accumulate(fitness.begin(), fitness.end(), 0.0);

    std::vector<std::vector<Point>> parents;
    for (int n = 0; n < params.pop_size; n++){
        double r = random_real(0, total);
        double cumulative = 0;
        size_t chosen = population.size() - 1;        // na wypadek bledow zaokraglen
        for (size_t i = 0; i < population.size(); i++){
            cumulative += fitness[i];
            if (r < cumulative){
                chosen = i;
                break;
            }
        }
        parents.push_back(population[chosen]);
    }
    return parents;
}

void Population::crossover(const std::vector<Point> &parent1, const std::vector<Point> &parent2,
                           std::vector<Point> &child1, std::vector<Point> &child2)
// krzyzowanie jednopunktowe z osobnym punktem ciecia w kazdym rodzicu
// (rodzice maja rozna dlugosc, wiec dzieci tez moga miec inna dlugosc)
{
    int cut1 = random_gen(parent1.size());
    int cut2 = random_gen(parent2.size());

    child1.assign(parent1.begin(), parent1.begin() + cut1);
    child1.insert(child1.end(), parent2.begin() + cut2, parent2.end());

    child2.assign(parent2.begin(), parent2.begin() + cut2);
    child2.insert(child2.end(), parent1.begin() + cut1, parent1.end());

    if ((int)child1.size() > max_path) child1.resize(max_path);
    if ((int)child2.size() > max_path) child2.resize(max_path);
}

void Population::clamp_to_map(Point &pt) const {
    const double eps = 1e-6;
    pt.xp = std::clamp(pt.xp, 0.0, map.get_width() - eps);
    pt.yp = std::clamp(pt.yp, 0.0, map.get_height() - eps);
}

void Population::mutation(std::vector<Point> &individual)
// mutacja genetyczna: kazdy gen (punkt) z prawdopodobienstwem params.mutation_prob
// przesuwamy o szum gaussowski; dodatkowo rzadko dodajemy lub usuwamy punkt,
// zeby zmieniala sie tez dlugosc chromosomu
{
    std::normal_distribution<> noise(0.0, params.mutation_sigma);
    for (Point &pt : individual){
        if (random_real(0, 1) < params.mutation_prob){
            pt.xp += noise(gen);
            pt.yp += noise(gen);
            clamp_to_map(pt);
        }
    }

    if ((int)individual.size() < max_path && random_real(0, 1) < params.insert_prob){
        // nowy punkt w poblizu srodka losowego odcinka sciezki
        std::vector<Point> path = full_path(individual);
        int k = random_gen(path.size() - 2);                  // odcinek path[k] -> path[k+1]
        Point pt((path[k].xp + path[k + 1].xp) / 2 + noise(gen),
                 (path[k].yp + path[k + 1].yp) / 2 + noise(gen));
        clamp_to_map(pt);
        individual.insert(individual.begin() + k, pt);
    }

    if (!individual.empty() && random_real(0, 1) < params.delete_prob){
        individual.erase(individual.begin() + random_gen(individual.size() - 1));
    }
}

std::vector<std::vector<Point>> Population::genetic_operations(const std::vector<std::vector<Point>> &parents)
// pula rodzicielska -> potomkowie (krzyzowanie kolejnych par, potem mutacja)
{
    std::vector<std::vector<Point>> offspring;
    for (size_t i = 0; i + 1 < parents.size(); i += 2){
        std::vector<Point> child1 = parents[i];
        std::vector<Point> child2 = parents[i + 1];
        if (random_real(0, 1) < params.crossover_prob)
            crossover(parents[i], parents[i + 1], child1, child2);
        offspring.push_back(child1);
        offspring.push_back(child2);
    }
    if (parents.size() % 2 == 1) offspring.push_back(parents.back());

    for (auto &child : offspring) mutation(child);
    return offspring;
}

void Population::replacement(std::vector<std::vector<Point>> &offspring)
// podstawienie: potomkowie zastepuja populacje, ale params.elite najlepszych
// osobnikow starej populacji zastepuje najgorszych potomkow (elitaryzm)
{
    std::vector<double> off_values, off_fitness;
    evaluate(offspring, off_values, off_fitness);

    std::vector<size_t> best_old(population.size());
    std::iota(best_old.begin(), best_old.end(), 0);
    std::sort(best_old.begin(), best_old.end(),
              [&](size_t a, size_t b){ return goal_values[a] < goal_values[b]; });

    std::vector<size_t> worst_new(offspring.size());
    std::iota(worst_new.begin(), worst_new.end(), 0);
    std::sort(worst_new.begin(), worst_new.end(),
              [&](size_t a, size_t b){ return off_values[a] > off_values[b]; });

    for (int e = 0; e < params.elite && e < (int)offspring.size(); e++){
        size_t dst = worst_new[e];
        offspring[dst] = population[best_old[e]];
        off_values[dst] = goal_values[best_old[e]];
        off_fitness[dst] = fitness[best_old[e]];
    }

    population.swap(offspring);
    goal_values.swap(off_values);
    fitness.swap(off_fitness);
}

void Population::next_generation(){
    std::vector<std::vector<Point>> parents = selection();
    std::vector<std::vector<Point>> offspring = genetic_operations(parents);
    replacement(offspring);
}

std::vector<Point> Population::best_path() const {
    size_t b = std::min_element(goal_values.begin(), goal_values.end()) - goal_values.begin();
    return full_path(population[b]);
}

double Population::best_value() const {
    return *std::min_element(goal_values.begin(), goal_values.end());
}

double Population::mean_value() const {
    return std::accumulate(goal_values.begin(), goal_values.end(), 0.0) / goal_values.size();
}

double Population::best_collision() const {
    std::vector<Point> path = best_path();
    Goal_function goal(path, map, params.collision_penalty);
    return goal.collision();
}

const std::vector<std::vector<Point>> &Population::get_population() const { return population; }
const std::vector<double> &Population::get_goal_values() const { return goal_values; }
const std::vector<double> &Population::get_fitness() const { return fitness; }
const Parameters &Population::get_parameters() const { return params; }
