#pragma once

#include <memory>
#include "EventBusImpl.h"

// Explanation for monstrosity that EventBus became
// Everything started as plain callback system (with addEventListener, removeEventListener, emit)
// But the problem was that it required explicit removal of listeners from user
// In practice this meant that he had to:
// 1. Store "key" of registered callback.
// 2. Remove it using removeEventListener method (from it's destructor)
// So, i have implemented system similar to unique_ptr:
// addEventListener returns "handle" that removes associated callback from it's own destructor (or it also have "remove" method if needed)
// So, now callback cleanup is automatic, and it forces user to store handles (otherwise callback will be removed instantly)
// To avoid situations when EventBus is destroyed, and
// EBHandle is destroyed after it (it would try to remove callback using pointer to invalidated EventBus)
// EBHandle stores weak_ptr, which allows to check if it still points to valid EventBus
// But getting weak_ptr from addEventListener required std::enable_shared_from_this
// But std::enable_shared_from_this requires that object is managed by shared_ptr.
// So, i had option to either force user to store EventBus in shared_ptr (which would be terrible)
// But i decided to wrap EventBus into another class which will store shared_ptr to it.
// Move assignment operator removes callback of object into which new object was moved to

// TODO there are still some problems and errors when including this file in "Level1" and "Game"

class EventBus {
private:
    std::shared_ptr<EventBusImpl> m_impl;
public:
    EventBus() : m_impl(std::make_shared<EventBusImpl>()) {};

    template<typename EventType>
    [[nodiscard]] EBHandle addEventListener(const std::function<void(const EventType &)> &callback) {
        return m_impl->addEventListener<EventType>(callback);
    };

    void emit(const BaseEvent &event) {
        m_impl->emit(event);
    };
};