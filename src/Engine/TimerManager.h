#pragma once

#include <unordered_map>
#include <functional>

class TimerManager {
    struct TickTimer {
        int ticks;
        int ticksLeft;
        std::function<void()> callback;

        TickTimer(int ticks, const std::function<void()>& callback) : ticks(ticks), ticksLeft(ticks), callback(callback) {

        }

        void tick() {
            ticksLeft--;
        }

        [[nodiscard]] bool isDone() const {
            return ticksLeft <= 0;
        }
    };

    std::unordered_map<int, TickTimer> tickTimers;
    std::unordered_map<int, TickTimer> tickIntervals;

    int keyCounter = 0;
public:
    void tick() {
        for (auto &pair : tickTimers) {
            pair.second.tick();
            if (pair.second.isDone()) {
                pair.second.callback();
                tickTimers.erase(pair.first);
            }
        }
        for (auto &pair : tickIntervals) {
            pair.second.tick();
            if (pair.second.isDone()) {
                pair.second.callback();
                pair.second.ticksLeft = pair.second.ticks;
            }
        }
    }

    int addTickTimer(int ticks, const std::function<void()>& callback) {
        tickTimers.emplace(keyCounter, TickTimer{ticks, callback});
        return keyCounter++;
    }

    void removeTickTimer(int key) {
        tickTimers.erase(key);
    }

    // TODO add interval timer

    int addTickInterval(int ticks, const std::function<void()>& callback) {
        tickIntervals.emplace(keyCounter, TickTimer{ticks, callback});
    }

    void removeTickInterval(int key) {
        tickIntervals.erase(key);
    }
};