#include "evopath/map.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace evo {

Map Map::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Nie mozna otworzyc mapy: " + path);
    std::stringstream ss;
    ss << in.rdbuf();
    return from_string(ss.str());
}

Map Map::from_string(const std::string& text) {
    Map m;
    std::istringstream in(text);
    std::string line;
    bool has_size = false, has_start = false, has_goal = false;
    int row = 0;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        // Komentarze '#' tylko przed linia 'size' - pozniej '#' to przeszkoda.
        if (line.empty() || (line[0] == '#' && !has_size)) continue;

        std::istringstream ls(line);
        std::string key;
        if (line[0] != '.' && line[0] != '#') {
            ls >> key;
            if (key == "size") {
                ls >> m.width_ >> m.height_;
                if (m.width_ <= 0 || m.height_ <= 0) throw std::runtime_error("Mapa: zly rozmiar");
                m.grid_.assign(static_cast<std::size_t>(m.width_) * m.height_, 0);
                has_size = true;
            } else if (key == "start") {
                ls >> m.start_.x >> m.start_.y;
                has_start = true;
            } else if (key == "goal") {
                ls >> m.goal_.x >> m.goal_.y;
                has_goal = true;
            } else {
                throw std::runtime_error("Mapa: nieznany klucz '" + key + "'");
            }
            continue;
        }

        if (!has_size) throw std::runtime_error("Mapa: brak linii 'size' przed siatka");
        if (row >= m.height_) throw std::runtime_error("Mapa: za duzo wierszy");
        if (static_cast<int>(line.size()) != m.width_)
            throw std::runtime_error("Mapa: wiersz " + std::to_string(row) + " ma zla dlugosc");
        for (int cx = 0; cx < m.width_; ++cx)
            m.grid_[static_cast<std::size_t>(row) * m.width_ + cx] = (line[cx] == '#') ? 1 : 0;
        ++row;
    }

    if (!has_size || !has_start || !has_goal) throw std::runtime_error("Mapa: brak size/start/goal");
    if (row != m.height_) throw std::runtime_error("Mapa: za malo wierszy");
    if (!m.is_free(m.start_.x, m.start_.y)) throw std::runtime_error("Mapa: start w przeszkodzie");
    if (!m.is_free(m.goal_.x, m.goal_.y)) throw std::runtime_error("Mapa: cel w przeszkodzie");
    return m;
}

bool Map::in_bounds(float x, float y) const {
    return x >= 0.0f && y >= 0.0f && x < static_cast<float>(width_) && y < static_cast<float>(height_);
}

bool Map::is_free(float x, float y) const {
    if (!in_bounds(x, y)) return false;
    const int cx = static_cast<int>(std::floor(x));
    const int cy = static_cast<int>(std::floor(y));
    return grid_[static_cast<std::size_t>(cy) * width_ + cx] == 0;
}

float Map::collision_length(Point a, Point b) const {
    // TODO(student): zwroc dlugosc czesci odcinka a-b, ktora lezy
    // w przeszkodach lub poza mapa. Np. probkowanie odcinka co krok < 1
    // komorki albo dokladne przejscie po komorkach (DDA / Bresenham).
    (void)a;
    (void)b;
    return 0.0f;
}

} // namespace evo
