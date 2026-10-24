# evopath — algorytm ewolucyjny do planowania ścieżki robota

Planowanie ścieżki robota mobilnego na dwuwymiarowej mapie z przeszkodami za pomocą
algorytmu ewolucyjnego. Obliczenia są w C++ (backend jednowątkowy CPU albo wielowątkowy
OpenMP), a eksperymenty, statystyki i wykresy w Pythonie. Oba światy łączy moduł pybind11.

```
Python (eksperymenty, wykresy, Jupyter)
        │  import evopath
Binding pybind11 (bindings/module.cpp)
        │
Rdzeń C++ (mapa, populacja, operatory, solver)
        │
Backend oceny: CPU / OpenMP
```

---

## 1. Wymagania

- Linux (testowane na Ubuntu 24.04)
- g++ z obsługą C++17 (OpenMP jest w zestawie z g++)
- CMake ≥ 3.18
- Python ≥ 3.8 z nagłówkami (`python3-dev`) i modułem `venv`
- opcjonalnie Ninja (szybsze budowanie) i VS Code z rozszerzeniem CMake Tools

## 2. Przygotowanie środowiska (jednorazowo)

```bash
# narzędzia systemowe
sudo apt update
sudo apt install build-essential cmake ninja-build python3-dev python3-venv

# wirtualne środowisko Pythona w katalogu projektu
cd ~/alg_gen
python3 -m venv .venv
source .venv/bin/activate
pip install pybind11 scikit-build-core numpy matplotlib pytest jupyter
```

> **W każdym nowym terminalu** zacznij od `source .venv/bin/activate`
> (w prompcie pojawi się `(.venv)`). Bez tego polecenie `python` nie zadziała,
> a `pip` odmówi instalacji (`externally-managed-environment`).

## 3. Budowanie

Pierwsza konfiguracja:

```bash
cmake -S . -B build -G Ninja -Dpybind11_DIR=$(python -m pybind11 --cmakedir)
```

Bez Ninja: pomiń `-G Ninja` (CMake użyje `make`).

Budowanie (po każdej zmianie w kodzie wystarczy tylko to):

```bash
cmake --build build
```

Powstaje:

| Plik | Co to jest |
|------|-----------|
| `build/libevo_core.a` | biblioteka z algorytmem |
| `build/run_ga` | program do uruchamiania GA z terminala |
| `build/test_*` | testy C++ |
| `python/evopath/_core.cpython-*.so` | moduł Pythona (ląduje od razu w pakiecie) |

Opcje CMake (dopisz do pierwszej konfiguracji, np. `-DEVO_ENABLE_OPENMP=OFF`):

| Opcja | Domyślnie | Znaczenie |
|-------|-----------|-----------|
| `EVO_ENABLE_OPENMP` | ON | backend wielowątkowy |
| `EVO_BUILD_PYTHON` | ON | moduł Pythona |
| `EVO_BUILD_APPS` | ON | program `run_ga` |
| `EVO_BUILD_TESTS` | ON | testy C++ |
| `CMAKE_BUILD_TYPE` | Release | `Debug` do debugowania (wolniej, z symbolami) |

Gdy coś pójdzie nie tak przy konfiguracji, wyczyść katalog i skonfiguruj od nowa:
`rm -rf build`.

## 4. Testy

```bash
ctest --test-dir build --output-on-failure    # wszystkie testy C++
./build/test_collision                         # jeden test
PYTHONPATH=python python -m pytest tests/python
```

| Test | Sprawdza | Stan w szkielecie |
|------|----------|-------------------|
| `test_map_loading` | wczytywanie mapy, `is_free` | przechodzi |
| `test_population` | układ danych populacji, statystyki historii | przechodzi |
| `test_collision` | `Map::collision_length` | **nie przechodzi**, dopóki go nie zaimplementujesz |
| `tests/python` | binding: mapa, solver, kształty tablic | przechodzi |

## 5. Uruchamianie

### 5.1. Z terminala (bez Pythona)

```bash
./build/run_ga <mapa> [pokolenia] [cpu|openmp] [seed]
./build/run_ga maps/simple.txt 500 openmp 42
```

Wypisuje backend, czas i najlepszy fitness oraz zapisuje:
- `results/best_path.csv` — punkty najlepszej ścieżki (`x,y`, ze startem i celem),
- `results/history.csv` — `generation,best,mean,worst` dla każdego pokolenia.

Porównanie CPU i OpenMP do sprawozdania:

```bash
./build/run_ga maps/simple.txt 500 cpu
./build/run_ga maps/simple.txt 500 openmp
```

### 5.2. Seria eksperymentów z pliku konfiguracyjnego

```bash
PYTHONPATH=python python python/run_experiment.py configs/default.json
```

Uruchamia GA `seeds` razy (różne ziarna), wypisuje statystyki
(najlepszy, średni, najgorszy wynik, odchylenie standardowe, średni czas)
i zapisuje wykresy do `results/<name>/`:
- `best_path.png` — mapa z najlepszą ścieżką,
- `convergence.png` — krzywe zbieżności,
- `snapshots.png` — populacja w wybranych pokoleniach (gdy `snapshot_every > 0`).

Własny eksperyment: skopiuj `configs/default.json`, zmień `name` i parametry.

### 5.3. Interaktywnie (Python / Jupyter)

```bash
PYTHONPATH=python jupyter notebook     # notatniki trzymaj w notebooks/
```

```python
import evopath
from evopath import plots

m = evopath.Map.load("maps/simple.txt")
cfg = evopath.make_config(population_size=300, mutation_prob=0.3,
                          backend="openmp", snapshot_every=20, seed=1)

solver = evopath.Solver(m, cfg)
solver.run(200)                 # można wołać wielokrotnie: solver.run(100) ...
print(solver.best_fitness, solver.generation)

plots.plot_path(m, solver.best_path(), title="Najlepsza ścieżka")
plots.plot_convergence(solver.history())
plots.plot_snapshots(m, solver.snapshots()[:6])

anim = plots.animate_population(m, solver.snapshots())
anim.save("results/ewolucja.gif", writer="pillow")

# 30 niezależnych uruchomień + statystyki
results = evopath.run_series(m, {"population_size": 200}, generations=500, seeds=range(30))
print(evopath.summarize(results))
```

Co zwraca `Solver`:

| Metoda / pole | Typ | Opis |
|---------------|-----|------|
| `run(n)` | — | wykonuje `n` pokoleń |
| `best_fitness`, `generation`, `backend` | liczba / tekst | stan solvera |
| `best_path()` | `ndarray (N, 2)` | najlepsza ścieżka ze startem i celem |
| `history()` | `dict` tablic | `generation`, `best`, `mean`, `worst` |
| `snapshots()` | lista `dict` | `generation`, `paths` (lista tablic `(n_i, 2)` z punktami pośrednimi) |
| `population()` | `dict` | aktualne `paths` i `fitness` |

### 5.4. Instalacja jako pakiet (opcjonalnie)

```bash
pip install .
```

Po tym `import evopath` działa bez `PYTHONPATH`, z dowolnego katalogu.
Po zmianach w C++ trzeba wtedy powtórzyć `pip install .`, dlatego podczas
pracy wygodniej używać `PYTHONPATH=python`.

## 6. Parametry (`configs/*.json`)

| Klucz | Domyślnie | Znaczenie |
|-------|-----------|-----------|
| `name` | `default` | nazwa eksperymentu (katalog w `results/`) |
| `map` | `maps/simple.txt` | plik mapy |
| `generations` | 500 | liczba pokoleń |
| `seeds` | 30 | liczba niezależnych uruchomień (ziarna 0…seeds−1) |
| `population_size` | 200 | rozmiar populacji |
| `max_points` | 32 | maks. liczba punktów pośrednich ścieżki |
| `initial_points` | 5 | liczba punktów przy inicjalizacji |
| `elite` | 2 | ilu najlepszych przechodzi bez zmian |
| `tournament_size` | 3 | rozmiar turnieju |
| `crossover_prob` | 0.9 | prawdopodobieństwo krzyżowania |
| `mutation_prob` | 0.2 | prawdopodobieństwo mutacji |
| `mutation_sigma` | 2.0 | odchylenie przesunięcia punktu [komórki] |
| `sigma_decay` | 1.0 | mnożnik sigma po każdym pokoleniu (np. 0.995) |
| `backend` | `openmp` | `cpu` albo `openmp` |
| `snapshot_every` | 25 | migawka populacji co N pokoleń (0 = brak) |
| `weights.length` | 1.0 | waga długości ścieżki |
| `weights.collision` | 100.0 | waga długości fragmentów w przeszkodach |
| `weights.smoothness` | 0.0 | waga kątów zakrętów |
| `weights.clearance` | 0.0 | waga kary za bliskość przeszkód |

Nowy parametr: dodaj pole w `core/include/evopath/config.hpp` i jedną linijkę
`def_readwrite` w `bindings/module.cpp` — wtedy można go ustawiać z JSON i Pythona.

## 7. Format mapy (`maps/*.txt`)

```
# komentarz (tylko przed linią 'size')
size 40 20
start 2.5 10.5
goal 37.5 10.5
........................................
............#...........................
...                                      <- dokładnie 20 wierszy po 40 znaków
```

- `.` — wolne pole, `#` — przeszkoda
- układ: x w prawo, y w dół; komórka `(cx, cy)` zajmuje `[cx, cx+1) × [cy, cy+1)`
- start i cel muszą leżeć w wolnym polu (inaczej `Map::load` zgłosi błąd)
- wszystko poza mapą traktowane jest jak przeszkoda

## 8. Gdzie implementujesz algorytm

Szkielet kompiluje się i działa, ale funkcje oznaczone `TODO(student)` są puste
(dlatego wszystkie wyniki wychodzą 0). Proponowana kolejność:

| # | Plik | Funkcja | Co zrobić |
|---|------|---------|-----------|
| 1 | `core/src/map.cpp` | `Map::collision_length` | długość części odcinka w przeszkodach / poza mapą; sprawdza ją `test_collision` |
| 2 | `backends/cpu/cpu_evaluator.cpp` | `evaluate_one` | funkcja przystosowania jednego osobnika |
| 3 | `core/src/operators.cpp` | `initialize` | populacja początkowa |
| 4 | `core/src/operators.cpp` | `tournament_select` | selekcja turniejowa |
| 5 | `core/src/operators.cpp` | `crossover` | krzyżowanie ścieżek o różnej długości |
| 6 | `core/src/operators.cpp` | `mutate_move`, `mutate_insert`, `mutate_delete`, `mutate_smooth`, `mutate` | mutacje i wybór mutacji |
| 7 | `core/src/solver.cpp` | `Solver::next_generation` | jedno pokolenie: elitaryzm, selekcja, krzyżowanie, mutacja, ocena |

Backend OpenMP (`backends/openmp/omp_evaluator.cpp`) jest gotowy — woła tę samą
`evaluate_one`, równolegle po osobnikach, więc zacznie działać po kroku 2.

Reprezentacja (`core/include/evopath/population.hpp`):

```cpp
struct Point      { float x, y; };
struct Individual { std::vector<Point> points; float fitness; };  // jedna ścieżka
using  Population = std::vector<Individual>;
```

Punkt `k` osobnika `i` to `pop[i].points[k]`, liczba punktów to `pop[i].points.size()`.
`points` to tylko punkty pośrednie — start i cel dokleja się przy ocenie
(pomocniczo `full_path(ind, map.start(), map.goal())`). Liczba punktów nie powinna
przekraczać `cfg.max_points`.

## 9. Struktura projektu

```
alg_gen/
├── CMakeLists.txt, pyproject.toml
├── core/
│   ├── include/evopath/   nagłówki: config, map, population, rng, history,
│   │                      operators, evaluator, solver
│   └── src/               map, history, operators, evaluator, solver
├── backends/
│   ├── cpu/               ocena jednowątkowa (tu jest evaluate_one)
│   └── openmp/            ocena wielowątkowa
├── bindings/module.cpp    moduł Pythona
├── apps/run_ga.cpp        program terminalowy
├── python/
│   ├── evopath/           pakiet: experiments.py, plots.py
│   └── run_experiment.py  seria uruchomień + wykresy
├── configs/               konfiguracje eksperymentów (JSON)
├── maps/                  mapy
├── notebooks/             notatniki Jupyter
├── results/               wyniki (nie trafiają do gita)
└── tests/                 cpp/ (ctest), python/ (pytest)
```

## 10. VS Code

Nie używaj przycisku „Run C/C++ File” — kompiluje pojedynczy plik bez CMake
i kończy się błędem `evopath/map.hpp: No such file or directory`.

Zainstaluj rozszerzenie **CMake Tools** i utwórz `.vscode/settings.json`:

```json
{
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "cmake.configureArgs": [
    "-Dpybind11_DIR=${workspaceFolder}/.venv/lib/python3.12/site-packages/pybind11/share/cmake/pybind11"
  ],
  "C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json"
}
```

(dopasuj `python3.12` do swojej wersji: `python3 --version`).
Na dolnym pasku są wtedy przyciski Build, Run/Debug (wybór celu, np. `test_collision`)
i Test. Do debugowania ustaw typ budowania `Debug`.

## 11. Najczęstsze problemy

| Komunikat | Przyczyna | Rozwiązanie |
|-----------|-----------|-------------|
| `Command 'python' not found` | nieaktywne venv | `source .venv/bin/activate` |
| `externally-managed-environment` | pip poza venv | jak wyżej |
| `CMAKE_CXX_COMPILER not set` | brak g++ | `sudo apt install build-essential`, potem `rm -rf build` |
| `unable to find ... "Ninja"` | brak Ninja | `sudo apt install ninja-build` albo bez `-G Ninja`, potem `rm -rf build` |
| `Could not find ... pybind11` | brak `-Dpybind11_DIR` lub venv | aktywuj venv, skonfiguruj z `-Dpybind11_DIR=$(python -m pybind11 --cmakedir)` |
| `Python.h: No such file` | brak nagłówków Pythona | `sudo apt install python3-dev` |
| `No module named 'evopath'` | Python nie widzi pakietu | uruchamiaj z `PYTHONPATH=python` z katalogu projektu lub `pip install .` |
| `No module named 'evopath._core'` | moduł niezbudowany | `cmake --build build` |
| `Nie mozna otworzyc mapy` | ścieżka względna | uruchamiaj z katalogu głównego projektu |
| wszystkie wyniki = 0 | algorytm niezaimplementowany | patrz sekcja 8 |
