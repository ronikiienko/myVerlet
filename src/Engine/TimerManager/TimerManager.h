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

    TMHandle setTimeout(int ticks, const std::function<void()>& callback) {
        return m_impl->setTimeout(ticks, callback);
    }

    TMHandle setInterval(int ticks, const std::function<void()>& callback) {
        return m_impl->setInterval(ticks, callback);
    }
};