#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace evo {

struct Point {
    float x = 0.0f;
    float y = 0.0f;
};

// Mapa jako siatka zajetosci. Uklad wspolrzednych: x w prawo (kolumny),
// y w dol (wiersze), komorka (cx, cy) zajmuje [cx, cx+1) x [cy, cy+1).
//
// Format pliku tekstowego (maps/*.txt):
//   # komentarz
//   size <szerokosc> <wysokosc>
//   start <x> <y>
//   goal <x> <y>
//   <wysokosc> wierszy po <szerokosc> znakow: '.' wolne, '#' przeszkoda
class Map {
public:
    static Map load(const std::string& path);
    static Map from_string(const std::string& text);

    int width() const { return width_; }
    int height() const { return height_; }
    Point start() const { return start_; }
    Point goal() const { return goal_; }

    // 1 = przeszkoda, 0 = wolne; indeks: cy * width + cx
    const std::vector<std::uint8_t>& grid() const { return grid_; }

    bool in_bounds(float x, float y) const;
    bool is_free(float x, float y) const;     // poza mapa = zajete

    // TODO(student): dlugosc fragmentu odcinka a-b lezacego w przeszkodach
    // (lub poza mapa). Implementacja: core/src/map.cpp
    float collision_length(Point a, Point b) const;

private:
    int width_ = 0;
    int height_ = 0;
    Point start_, goal_;
    std::vector<std::uint8_t> grid_;
};

} // namespace evo
