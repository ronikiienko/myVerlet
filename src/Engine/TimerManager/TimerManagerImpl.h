#pragma once

#include <unordered_map>
#include <functional>
#include <memory>
#include "TMHandle.h"

// better not use it too much because it uses callbacks. so not use for each of for example 50 000 objects lol
class TimerManagerImpl : public std::enable_shared_from_this<TimerManagerImpl> {
    struct TickTimer {
        int m_ticks;
        int m_ticksLeft;
        std::function<void()> m_callback;

        template<typename T>
        TickTimer(int ticks, T&& callback) : m_ticks(ticks), m_ticksLeft(ticks), m_callback(std::forward<T>(callback)) {

        }

        void tick() {
            m_ticksLeft--;
        }

        [[nodiscard]] bool isDone() const {
            return m_ticksLeft <= 0;
        }
    };

    std::unordered_map<int, TickTimer> m_tickTimers;
    std::unordered_map<int, TickTimer> m_tickIntervals;

    int m_keyCounter = 0;
public:
    TimerManagerImpl() = default;

    void tick() {
        for (auto &pair : m_tickTimers) {
            pair.second.tick();
            if (pair.second.isDone()) {
                pair.second.m_callback();
                m_tickTimers.erase(pair.first);
            }
        }
        for (auto &pair : m_tickIntervals) {
            pair.second.tick();
            if (pair.second.isDone()) {
                pair.second.m_callback();
                pair.second.m_ticksLeft = pair.second.m_ticks;
            }
        }
    }

    template<typename T>
    [[nodiscard]] TMHandle setTimeout(int ticks, T&& callback) {
        m_tickTimers.emplace(m_keyCounter, TickTimer{ticks, std::forward<T>(callback)});
        return {weak_from_this(), false, m_keyCounter++};
    }

    void removeTimeout(int key) {
        m_tickTimers.erase(key);
    }

    // TODO add m_interval timer
    template<typename T>
    [[nodiscard]] TMHandle setInterval(int ticks, T&& callback) {
        m_tickIntervals.emplace(m_keyCounter, TickTimer{ticks, std::forward<T>(callback)});
        return {weak_from_this(), true, m_keyCounter++};
    }

    void removeInterval(int key) {
        m_tickIntervals.erase(key);
    }

    TimerManagerImpl(const TimerManagerImpl &) = delete;
    TimerManagerImpl& operator=(const TimerManagerImpl &) = delete;
    TimerManagerImpl(TimerManagerImpl &&) = delete;
    TimerManagerImpl& operator=(TimerManagerImpl &&) = delete;
};