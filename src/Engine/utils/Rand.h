#pragma once

#include <random>

class NumberGenerator {
protected:
    std::random_device m_rd;
    std::mt19937 m_gen;

    NumberGenerator() : m_gen(m_rd()) {};

    explicit NumberGenerator(int seed) : m_gen(seed) {};
};

template<typename T>
class RealNumberGenerator : NumberGenerator {
private:
    std::uniform_real_distribution<T> m_distribution{0, 1};
public:
    explicit RealNumberGenerator(int seed) : NumberGenerator(seed) {}
    RealNumberGenerator() : NumberGenerator() {}

    T get() {
        return m_distribution(m_gen);
    }

    T getInRange(T min, T max) {
        return min + get() * (max - min);
    }
};

template<typename T>
class IntNumberGenerator : NumberGenerator {
public:
    explicit IntNumberGenerator(int seed) : NumberGenerator(seed) {}
    IntNumberGenerator() : NumberGenerator() {}

    T getInRange(T min, T max) {
        std::uniform_int_distribution<T> distribution(min, max);
        return distribution(m_gen);
    }
};

using RNGf = RealNumberGenerator<float>;
using RNGd = RealNumberGenerator<double>;
using RNGi = IntNumberGenerator<int>;
using RNGs = IntNumberGenerator<short>;
