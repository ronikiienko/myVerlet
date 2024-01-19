#pragma once
#include <memory>
#include "TimerManagerImpl.h"

class TimerManagerImpl;
class TMHandle {
private:
    int m_key = -1;
    bool m_isInterval = false;
    std::weak_ptr<TimerManagerImpl> m_timerManagerPtr;

public:
    TMHandle() = default;
    TMHandle(std::weak_ptr<TimerManagerImpl> eventBusPtr, bool isInterval, int key);

    void remove();
    bool isOwner();
    ~TMHandle();

    TMHandle (const TMHandle&) = delete;
    TMHandle& operator= (const TMHandle&) = delete;

    TMHandle(TMHandle&&) noexcept;
    TMHandle& operator= (TMHandle&&) noexcept;
};