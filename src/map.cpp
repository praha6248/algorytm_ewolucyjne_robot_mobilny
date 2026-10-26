#include "map.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

Map::Map()
: width(0), height(0) {}

void Map::load(const std::string &file_name) {
    std::ifstream in(file_name);
    if (!in) throw std::runtime_error("Nie mozna otworzyc mapy: " + file_name);

    bool has_size = false, has_start = false, has_goal = false;
    int row = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || (line[0] == '#' && !has_size)) continue;

        if (line[0] != '.' && line[0] != '#') {
            std::istringstream ls(line);
            std::string key;
            ls >> key;
            if (key == "size") {
                ls >> width >> height;
                grid.assign(width * height, 0);
                has_size = true;
            } else if (key == "start") {
                ls >> start.xp >> start.yp;
                has_start = true;
            } else if (key == "goal") {
                ls >> goal.xp >> goal.yp;
                has_goal = true;
            } else {
                throw std::runtime_error("Mapa: nieznany klucz " + key);
            }
            continue;
        }

        if (!has_size || row >= height || (int)line.size() != width)
            throw std::runtime_error("Mapa: zly wiersz " + std::to_string(row));
        for (int cx = 0; cx < width; cx++)
            grid[row * width + cx] = (line[cx] == '#') ? 1 : 0;
        row++;
    }

    if (!has_size || !has_start || !has_goal || row != height)
        throw std::runtime_error("Mapa: brak size/start/goal lub zla liczba wierszy");
    if (!is_free(start.xp, start.yp) || !is_free(goal.xp, goal.yp))
        throw std::runtime_error("Mapa: start lub cel w przeszkodzie");
}

int Map::get_width() const { return width; }
int Map::get_height() const { return height; }
Point Map::get_start() const { return start; }
Point Map::get_goal() const { return goal; }
const std::vector<char> &Map::get_grid() const { return grid; }

bool Map::is_free(double x, double y) const {
    if (x < 0 || y < 0 || x >= width || y >= height) return false;
    int cx = (int)std::floor(x);
    int cy = (int)std::floor(y);
    return grid[cy * width + cx] == 0;
}

double Map::collision_length(Point a, Point b) const
// probkowanie odcinka co ok. 0.1 komorki; sumujemy kroki lezace w przeszkodzie
{
    const double dx = b.xp - a.xp;
    const double dy = b.yp - a.yp;
    const double len = std::sqrt(dx * dx + dy * dy);
    if (len == 0.0) return 0.0;              // odcinek zerowej dlugosci

    const int steps = (int)std::ceil(len / 0.1);
    const double step_len = len / steps;
    double sum = 0.0;
    for (int s = 0; s < steps; s++) {
        double t = (s + 0.5) / steps;            // srodek kroku
        if (!is_free(a.xp + t * dx, a.yp + t * dy)) sum += step_len;
    }
    return sum;
}
