# Algorytm ewolucyjny — planowanie ścieżki robota mobilnego

Zadanie 4a: ścieżka jako sekwencja punktów we współrzędnych kartezjańskich (x, y).
Funkcja celu uwzględnia długość ścieżki i kolizje z przeszkodami.

## Cykl ewolucyjny

Zgodnie ze schematem z wykładu (`Population::next_generation`):

| Krok schematu | Funkcja | Opis |
|---|---|---|
| Populacja osobników | konstruktor `Population` | `pop_size` osobników, każdy to losowa liczba (0…`max_path`) losowych punktów na mapie |
| Funkcja celu → stopień przystosowania | `Population::evaluate`, `Goal_function::value` | wartość = długość + `collision_penalty` × długość w przeszkodach; przystosowanie = 1 / (1 + wartość) |
| Selekcja | `Population::selection` | ruletka: prawdopodobieństwo wyboru proporcjonalne do przystosowania |
| Operacje genetyczne | `Population::genetic_operations` | krzyżowanie jednopunktowe (`crossover`) i mutacja (`mutation`) |
| Podstawienie | `Population::replacement` | potomkowie zastępują populację; `elite` najlepszych starych osobników zastępuje najgorszych potomków |

Osobnik przechowuje tylko punkty pośrednie. Start i cel są wspólne
dla wszystkich osobników i dokleja się je przy liczeniu funkcji celu.

**Mutacja:** każdy gen (punkt) z prawdopodobieństwem `mutation_prob` jest przesuwany
o szum gaussowski (`mutation_sigma`). Rzadziej dodawany jest nowy punkt (`insert_prob`)
albo usuwany istniejący (`delete_prob`), żeby zmieniała się też długość chromosomu.

**Krzyżowanie:** każdy rodzic jest cięty w innym miejscu, więc dzieci mogą mieć
inną liczbę punktów niż rodzice (najwyżej `max_path`).

Parametry są w strukturze `Parameters` (`inc/population.hpp`), domyślne wartości
to te same liczby co wcześniej. Można je zmieniać z Pythona bez rekompilacji.

## Pliki

```
inc/point.h, src/point.cpp                   punkt (xp, yp)
inc/map.hpp, src/map.cpp                     mapa z przeszkodami, kolizje odcinka
inc/goal_function.hpp, src/goal_function.cpp funkcja celu
inc/population.hpp, src/population.cpp       populacja i cykl AG
src/main.cpp                                 program terminalowy, zapis CSV
bindings/module.cpp                          moduł Pythona (pybind11)
python/robot_ga/                             pakiet Pythona: experiments.py, plots.py
python/example.py                            przykład użycia biblioteki
maps/map1.txt                                przykładowa mapa
plot.py                                      wykresy z plików CSV programu terminalowego
```

## Instalacja (jednorazowo)

```bash
sudo apt install build-essential cmake python3-dev python3-venv
python3 -m venv .venv
source .venv/bin/activate              # w każdym nowym terminalu
pip install pybind11 scikit-build-core numpy matplotlib
```

## Biblioteka Pythona

Budowanie podczas pracy nad kodem (moduł ląduje w `python/robot_ga/`):

```bash
cmake -S . -B build -Dpybind11_DIR=$(python -m pybind11 --cmakedir)
cmake --build build                    # po każdej zmianie w C++
PYTHONPATH=python python python/example.py
```

Albo instalacja jako pakiet (`import robot_ga` działa wtedy z dowolnego miejsca,
po zmianach w C++ trzeba powtórzyć):

```bash
pip install .
```

Użycie:

```python
import robot_ga
from robot_ga import plots

m = robot_ga.Map("maps/map1.txt")

# jedno uruchomienie; parametry jak w Parameters, np. mutation_prob, pop_size
r = robot_ga.run(m, generations=300, max_path=8, seed=1,
                 mutation_prob=0.15, snapshot_every=50)
print(r["best_value"], r["best_collision"], r["time_s"])

plots.plot_path(m, r["best_path"])
plots.plot_convergence(r["history"])
plots.plot_snapshots(m, r["snapshots"])
plots.animate(m, r["snapshots"]).save("results/ewolucja.gif", writer="pillow")

# 30 uruchomień + statystyki (best, mean, worst, std, odsetek bez kolizji)
series = robot_ga.run_series(m, seeds=range(30), generations=300)
print(robot_ga.summarize(series))

# krok po kroku, bezpośrednio na klasie Population
pop = robot_ga.Population(m, max_path=8, seed=1,
                          params=robot_ga.make_parameters(pop_size=200))
for g in range(100):
    pop.next_generation()        # selekcja -> operacje genetyczne -> podstawienie
print(pop.best_value(), pop.best_path())
plots.plot_population(m, pop.individuals())
```

| Element | Opis |
|---|---|
| `Map(plik)` | `width`, `height`, `start`, `goal`, `grid` (numpy), `is_free(x, y)`, `collision_length(...)` |
| `Parameters` / `make_parameters(**kw)` | `pop_size`, `crossover_prob`, `mutation_prob`, `mutation_sigma`, `insert_prob`, `delete_prob`, `elite`, `collision_penalty` |
| `Population(map, max_path, seed, params)` | `next_generation()`, `run(n)`, `best_path()`, `best_value()`, `mean_value()`, `best_collision()`, `individuals()`, `goal_values()`, `fitness()` |
| `goal_value(path, map, penalty)` | (wartość, długość, kolizje) dla dowolnej pełnej ścieżki `(n, 2)` |
| `run`, `run_series`, `summarize` | eksperymenty i statystyki |
| `plots.*` | `plot_map`, `plot_path`, `plot_convergence`, `plot_population`, `plot_snapshots`, `animate` |

## Program terminalowy

```bash
cmake --build build

./build/robot_ga [mapa] [pokolenia] [max_punktow] [seed]
./build/robot_ga maps/map1.txt 300 8 1

python3 plot.py maps/map1.txt
```

Program zapisuje `results/best_path.csv` i `results/history.csv`, a `plot.py`
tworzy z nich `results/best_path.png` (mapa z trasą) i `results/convergence.png`
(najlepsza i średnia wartość funkcji celu w kolejnych pokoleniach).
Uruchamiaj z katalogu głównego projektu.

## Format mapy

```
# komentarz (tylko przed linią size)
size 60 40
start 5.5 20.5
goal 55.5 20.5
....##....      <- 40 wierszy po 60 znaków: '.' wolne, '#' przeszkoda
```

Układ: x w prawo, y w dół; komórka (cx, cy) zajmuje [cx, cx+1) × [cy, cy+1).
Obszar poza mapą liczy się jak przeszkoda.
