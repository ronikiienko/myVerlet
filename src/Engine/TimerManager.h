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

    int keyCounter = 0;
public:
    void update() {
        for (auto &pair : tickTimers) {
            pair.second.tick();
            if (pair.second.isDone()) {
                pair.second.callback();
                tickTimers.erase(pair.first);
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
};