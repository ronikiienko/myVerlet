#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <chrono>

class PerformanceMonitor {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    std::unordered_map<std::string, TimePoint> m_clocks;
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
        m_clocks[label] = Clock::now();
    }

    void end(const std::string &label) {
        if (!m_clocks.count(label)) {
            throw std::runtime_error("Trying to end timer that wasn't started (PerformanceMonitor)" + label);
        }
        const long long elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - m_clocks[label]).count();

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

