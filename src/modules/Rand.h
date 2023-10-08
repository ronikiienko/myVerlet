#pragma once

#include <random>

class NumberGenerator {
protected:
    std::random_device rd;
    std::mt19937 gen;

    NumberGenerator() : gen(rd()) {};

    explicit NumberGenerator(int seed) : gen(seed) {};
};

class RNGf : NumberGenerator {
private:
    std::uniform_real_distribution<float> distribution;
public:
    RNGf() : NumberGenerator(), distribution(0.0f, 1.0f) {};

    explicit RNGf(int seed) : NumberGenerator(seed), distribution(0.0f, 1.0f) {};

    float get() {
        return distribution(gen);
    }

    float getInRange(float min, float max) {
        return min + get() * (max - min);
    }
};