#include <inc/goal_function.hpp>

Goal_function::Goal_function(std::vector<Point> &path)
: robot_path(path)
 {}

double Goal_function::value()
// objective function
{
    double sum = 0;
    auto euclidean_distance = [] (Point pt1, Point pt2) {
        return sqrt(pt1.xp * pt2.xp + pt1.yp * pt2.yp);
    };
    
    for (auto it = robot_path.begin(); it != std::prev(robot_path.end()); ++it) {
        auto next = *std::next(it);
        sum += euclidean_distance(*it, next);
    }
    
    return sum;
}