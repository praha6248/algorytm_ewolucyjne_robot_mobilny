// Reprezentacja populacji i historia - powinien przechodzic od razu.
#include "check.hpp"
#include "evopath/history.hpp"
#include "evopath/population.hpp"

using namespace evo;

int main() {
    Population pop(3);
    CHECK(pop.size() == 3u);
    CHECK(pop[0].points.empty());

    pop[1].points = {{1.0f, 2.0f}, {3.0f, 4.0f}};
    CHECK(pop[1].points.size() == 2u);
    CHECK(pop[1].points[1].y == 4.0f);

    pop[2] = pop[1];                       // kopia osobnika
    pop[2].points.push_back({5.0f, 6.0f});
    CHECK(pop[1].points.size() == 2u);      // oryginal bez zmian
    CHECK(pop[2].points.size() == 3u);

    const std::vector<Point> path = full_path(pop[1], {0.0f, 0.0f}, {9.0f, 9.0f});
    CHECK(path.size() == 4u);
    CHECK(path.front().x == 0.0f && path.back().x == 9.0f);

    pop[0].fitness = 5.0f;
    pop[1].fitness = 1.0f;
    pop[2].fitness = 3.0f;
    CHECK(best_index(pop) == 1);

    History h;
    h.record(pop, 0, true);
    CHECK_NEAR(h.best[0], 1.0, 1e-6);
    CHECK_NEAR(h.mean[0], 3.0, 1e-6);
    CHECK_NEAR(h.worst[0], 5.0, 1e-6);
    CHECK(h.snapshots.size() == 1u);
    CHECK(h.snapshots[0].paths[2].size() == 3u);
    return TEST_RESULT();
}
