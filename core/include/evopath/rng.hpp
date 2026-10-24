#pragma once
#include <cstdint>
#include <random>

namespace evo {

// Generator liczb losowych z ziarnem - jedno zrodlo losowosci dla calego GA,
// zeby wyniki byly powtarzalne.
class Rng {
public:
    explicit Rng(std::uint64_t seed = 42) : gen_(seed) {}

    void seed(std::uint64_t s) { gen_.seed(s); }

    float uniform(float lo = 0.0f, float hi = 1.0f) {
        return std::uniform_real_distribution<float>(lo, hi)(gen_);
    }
    float normal(float mean = 0.0f, float sigma = 1.0f) {
        return std::normal_distribution<float>(mean, sigma)(gen_);
    }
    // Liczba calkowita z przedzialu [lo, hi] (wlacznie).
    int randint(int lo, int hi) {
        return std::uniform_int_distribution<int>(lo, hi)(gen_);
    }
    bool chance(float p) { return uniform() < p; }

    std::mt19937_64& engine() { return gen_; }

private:
    std::mt19937_64 gen_;
};

} // namespace evo
