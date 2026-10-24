// Geometria kolizji - NIE PRZEJDZIE, dopoki nie zaimplementujesz
// Map::collision_length w core/src/map.cpp. To jest Twoj test "na zielono".
#include "check.hpp"
#include "evopath/map.hpp"

using namespace evo;

int main() {
    // Przeszkoda: kolumna x in [2, 3), cala wysokosc.
    const Map m = Map::from_string(
        "size 5 3\n"
        "start 0.5 1.5\n"
        "goal 4.5 1.5\n"
        "..#..\n"
        "..#..\n"
        "..#..\n");
    const float tol = 0.1f;   // dopuszczalny blad przy probkowaniu

    // Odcinek calkowicie w wolnym polu
    CHECK_NEAR(m.collision_length({0.5f, 0.5f}, {1.5f, 2.5f}), 0.0, tol);
    // Poziomo przez przeszkode: 1 komorka
    CHECK_NEAR(m.collision_length({0.5f, 1.5f}, {4.5f, 1.5f}), 1.0, tol);
    // Odcinek w calosci w przeszkodzie
    CHECK_NEAR(m.collision_length({2.2f, 0.5f}, {2.2f, 2.5f}), 2.0, tol);
    // Ukosnie przez przeszkode: dlugosc w kolumnie = sqrt(1 + 0.5^2)
    CHECK_NEAR(m.collision_length({0.0f, 0.5f}, {4.0f, 2.5f}), 1.118, tol);
    // Wyjscie poza mape liczy sie jak kolizja
    CHECK_NEAR(m.collision_length({0.5f, 1.5f}, {0.5f, -0.5f}), 0.5, tol);
    return TEST_RESULT();
}
