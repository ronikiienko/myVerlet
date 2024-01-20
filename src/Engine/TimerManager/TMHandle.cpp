#include "TMHandle.h"

TMHandle::TMHandle(std::weak_ptr<TimerManagerImpl> timerManagerPtr, bool isInterval, int key) : m_key(key), m_isInterval(isInterval),
                                                                                             m_timerManagerPtr(std::move(
                                                                                                     timerManagerPtr)) {
}

void TMHandle::remove() {
    if (auto timerManagerPtr = m_timerManagerPtr.lock()) {
        if (m_isInterval) {
            timerManagerPtr->removeInterval(m_key);
        } else {
            timerManagerPtr->removeTimeout(m_key);
        }
        m_timerManagerPtr = std::shared_ptr<TimerManagerImpl>();
    }
};

TMHandle::~TMHandle() {
    remove();
}

TMHandle &TMHandle::operator=(TMHandle && other) noexcept {
    if (this != &other) {
        remove();
        m_key = other.m_key;
        m_isInterval = other.m_isInterval;
        m_timerManagerPtr = other.m_timerManagerPtr;
        other.m_timerManagerPtr = std::weak_ptr<TimerManagerImpl>();
    }
    return *this;
}

TMHandle::TMHandle(TMHandle && other) noexcept {
    m_key = other.m_key;
    m_isInterval = other.m_isInterval;
    m_timerManagerPtr = other.m_timerManagerPtr;
    other.m_timerManagerPtr = std::weak_ptr<TimerManagerImpl>();
}

bool TMHandle::isOwner() {
    return !m_timerManagerPtr.expired();
}

