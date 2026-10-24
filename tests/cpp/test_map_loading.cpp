// Wczytywanie mapy - powinien przechodzic od razu.
#include "check.hpp"
#include "evopath/map.hpp"

using namespace evo;

int main() {
    const Map m = Map::from_string(
        "size 4 3\n"
        "start 0.5 0.5\n"
        "goal 3.5 2.5\n"
        "..#.\n"
        "..#.\n"
        "....\n");
    CHECK(m.width() == 4);
    CHECK(m.height() == 3);
    CHECK(m.is_free(0.5f, 0.5f));
    CHECK(!m.is_free(2.5f, 0.5f));
    CHECK(m.is_free(2.5f, 2.5f));
    CHECK(!m.is_free(-0.1f, 1.0f));   // poza mapa = zajete
    CHECK(!m.is_free(4.0f, 1.0f));

    const Map file = Map::load("maps/simple.txt");
    CHECK(file.width() > 0);
    CHECK(file.is_free(file.start().x, file.start().y));
    return TEST_RESULT();
}
