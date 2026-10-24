#pragma once
#include <cstdint>

namespace evo {

enum class Backend { CPU, OpenMP };

// Wagi skladnikow funkcji przystosowania:
//   F = length * L + collision * K + smoothness * G + clearance * O
struct FitnessWeights {
    float length     = 1.0f;
    float collision  = 100.0f;
    float smoothness = 0.0f;
    float clearance  = 0.0f;
};

struct GAConfig {
    int   population_size = 200;
    int   max_points      = 32;    // maks. liczba punktow posrednich w chromosomie
    int   initial_points  = 5;     // liczba punktow posrednich przy inicjalizacji
    int   elite           = 2;     // ilu najlepszych przechodzi bez zmian
    int   tournament_size = 3;

    float crossover_prob  = 0.9f;
    float mutation_prob   = 0.2f;
    float mutation_sigma  = 2.0f;  // odchylenie przesuniecia punktu [komorki mapy]
    float sigma_decay     = 1.0f;  // mnoznik sigma po kazdym pokoleniu (1 = stale)

    std::uint64_t seed    = 42;
    Backend backend       = Backend::CPU;
    int   snapshot_every  = 0;     // migawka populacji co N pokolen (0 = wylaczone)

    FitnessWeights weights;
};

} // namespace evo
