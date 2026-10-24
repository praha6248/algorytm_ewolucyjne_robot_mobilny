#include <population.hpp>

const int POP_SIZE = 100;
const int MAP_SIZE = 100;

Population::Population(int max_point, std::vector<Point> &path)
: max_path(max_point), robot_path(path){

    for (int i = 0; i < POP_SIZE; i++){
        int path_size = random_gen(max_path);
        for (int j = 0; j < path_size; j++){
            Point pt;
            pt.xp = random_gen(MAP_SIZE);
            pt.yp = random_gen(MAP_SIZE);
            robot_path.push_back(pt);
        }
        population.push_back(robot_path);
    }
}

int Population::random_gen(int max){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(0, max);

    int random_nr = dist(gen);

    return random_nr;
}

void Population::selection() {
    for(auto it: population){
        Goal_function goal(it);
        goal.value()
    }
}
