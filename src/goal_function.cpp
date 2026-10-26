#include "goal_function.hpp"

#include <iterator>

Goal_function::Goal_function(std::vector<Point> &path)
: robot_path(path), map(nullptr), penalty(COLLISION_PENALTY)
 {}

Goal_function::Goal_function(std::vector<Point> &path, const Map &m, double p)
: robot_path(path), map(&m), penalty(p)
 {}

double Goal_function::length()
// suma dlugosci odcinkow sciezki
{
    double sum = 0;
    auto euclidean_distance = [] (Point pt1, Point pt2) {
        double dx = pt2.xp - pt1.xp;
        double dy = pt2.yp - pt1.yp;
        return std::sqrt(dx * dx + dy * dy);
    };

    if (robot_path.size() < 2) return 0.0;
    for (auto it = robot_path.begin(); it != std::prev(robot_path.end()); ++it) {
        auto next = *std::next(it);
        sum += euclidean_distance(*it, next);
    }

    return sum;
}

double Goal_function::collision()
// suma dlugosci fragmentow sciezki lezacych w przeszkodach lub poza mapa
{
    if (map == nullptr || robot_path.size() < 2) return 0.0;
    double sum = 0;
    for (auto it = robot_path.begin(); it != std::prev(robot_path.end()); ++it) {
        sum += map->collision_length(*it, *std::next(it));
    }
    return sum;
}

double Goal_function::value()
// objective function
{
    return length() + penalty * collision();
}
