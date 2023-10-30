#pragma once

#include <vector>
#include <numeric>
#include <iostream>
#include "SFML/System/Clock.hpp"

class Benchmark {
private:
    bool resultShown = false;
    int samplesNum;
    int nextSampleIndex = 0;
    std::vector<long long> samples;
    sf::Clock clock;
public:
    explicit Benchmark(int samplesNum) : samplesNum(samplesNum) {
        samples.resize(samplesNum);
    };


    void start() {
        clock.restart();
    }

    void outputResults() {
        if (resultShown) return;
        long long sum = 0;
        long long avg;
        for (auto& sample : samples) {
            sum += sample;
        }

        avg = sum /  static_cast<long long>(samples.size());

        double avgFloat = static_cast<double>(avg);

        std::cout << "Avg time per frame is: " << avgFloat / 1000 << " MS";
        std::cout << "Avg FPS is: " << 1000000 / avgFloat << " MS";

        resultShown = true;
    }

    void sample() {
        if (nextSampleIndex < samplesNum) {
            const long long elapsed = clock.restart().asMicroseconds();
            samples[nextSampleIndex] = elapsed;
            nextSampleIndex++;
        } else {
            outputResults();
        }
    }
};