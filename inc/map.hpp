#pragma once
#include <string>
#include <vector>
#include "point.h"

// Mapa jako siatka zajetosci.
// Uklad: x w prawo (kolumny), y w dol (wiersze);
// komorka (cx, cy) zajmuje [cx, cx+1) x [cy, cy+1).
//
// Format pliku (maps/*.txt):
//   # komentarz (tylko przed linia size)
//   size <szerokosc> <wysokosc>
//   start <x> <y>
//   goal <x> <y>
//   <wysokosc> wierszy po <szerokosc> znakow: '.' wolne, '#' przeszkoda
class Map {
private:
    int width;
    int height;
    Point start;
    Point goal;
    std::vector<char> grid;   // 1 = przeszkoda; indeks cy * width + cx
public:
    Map();
    void load(const std::string &file_name);

    int get_width() const;
    int get_height() const;
    Point get_start() const;
    Point get_goal() const;
    const std::vector<char> &get_grid() const;              // 1 = przeszkoda, indeks cy * width + cx

    bool is_free(double x, double y) const;                 // poza mapa = zajete
    double collision_length(Point a, Point b) const;        // dlugosc odcinka w przeszkodach
};
