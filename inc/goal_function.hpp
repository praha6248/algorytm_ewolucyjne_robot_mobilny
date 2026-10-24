#include <inc/point.h>
#include <vector>
#include <cmath>

class Goal_function {
private:
    std::vector<Point> robot_path;
public:
    Goal_function(std::vector<Point> &path);
    double value();
};