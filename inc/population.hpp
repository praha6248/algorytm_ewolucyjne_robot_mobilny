#include <random>
#include <point.h>
#include <goal_function.hpp>
class Population {
private:
    int max_path;
    std::vector<Point> robot_path;
    std::vector<std::vector<Point>> population;
public:
    Population(int, std::vector<Point> &);
    int random_gen(int);
    void selection();
};
