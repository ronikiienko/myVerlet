#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "Scene.h"

class PerformanceMonitor {
private:
    std::unordered_map<std::string, sf::Clock> m_clocks;
    std::unordered_map<std::string, long long> m_times;
    std::string m_string = "uninitialized";

    int m_frameCount = 0;
    int m_sampleFrameSize = 60;
public:
    explicit PerformanceMonitor() = default;

    void startFrame() {
        if (m_frameCount >= m_sampleFrameSize) {
            m_string = "";
            for (auto &it: m_times) {
                double milliseconds = static_cast<double>(it.second) / 1000 / m_frameCount;
                short fps = 1000 / milliseconds;
                m_string += '\n' + it.first + ":   " + std::to_string(milliseconds) + " ms" + "    /    " +
                            std::to_string(fps) + " FPS";
            }

            for (auto &it: m_times) {
                it.second = 0;
            }
            m_frameCount = 0;
        } else {
            m_frameCount++;
        }
    }

    void setSampleFrameSize(int size) {
        m_sampleFrameSize = size;
    }

    void start(const std::string &label) {
        if (!m_clocks.count(label)) {
            m_clocks.insert({label, sf::Clock{}});
        }
        sf::Clock &clock = m_clocks.find(label)->second;
        clock.restart();
    }

    void end(const std::string &label) {
        if (!m_clocks.count(label)) {
            throw std::runtime_error("Trying to end timer that wasn't started (PerformanceMonitor)" + label);
        }
        sf::Clock &clock = m_clocks.find(label)->second;
        const long long elapsed = clock.restart().asMicroseconds();

        if (!m_times.count(label)) {
            m_times.insert({label, 0.0f});
        }
        m_times[label] += elapsed;
    }

    void remove(const std::string &label) {
        m_clocks.erase(label);
        m_times.erase(label);
    }

    std::string getString() {
        return m_string;
    }

    PerformanceMonitor(const PerformanceMonitor &) = delete;

    PerformanceMonitor &operator=(const PerformanceMonitor &) = delete;

    PerformanceMonitor(PerformanceMonitor &&) = delete;

    PerformanceMonitor &operator=(PerformanceMonitor &&) = delete;
};

