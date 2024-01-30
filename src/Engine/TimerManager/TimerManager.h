#pragma once

#include "TimerManagerImpl.h"

class TimerManager {
private:
    std::shared_ptr<TimerManagerImpl> m_impl;
public:
    TimerManager() : m_impl(std::make_shared<TimerManagerImpl>()) {};

    void tick() {
        m_impl->tick();
    }

    template<typename T>
    [[nodiscard]] TMHandle setTimeout(int ticks, T&& callback) {
        return m_impl->setTimeout(ticks, std::forward<T>(callback));
    }

    template<typename T>
    [[nodiscard]] TMHandle setInterval(int ticks, T&& callback) {
        return m_impl->setInterval(ticks, std::forward<T>(callback));
    }
};