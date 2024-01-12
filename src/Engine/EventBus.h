#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

struct BaseEvent {
    BaseEvent() = default;
    virtual ~BaseEvent() = default;
};

class EventBus {
    std::unordered_map<std::type_index, std::unordered_map<int, std::function<void(const BaseEvent &)>>> m_callbacks;
    int m_keyCounter = 0;
public:
    EventBus() = default;

    template<typename EventType>
    int addEventListener(const std::function<void(const EventType &)> &callback) {
        // here passing m_callback as copy is necessary. otherwise if parameter is a reference and it is destroyed, lambda will have dangling reference
        auto wrapper = [callback](const BaseEvent &baseEvent) {
            callback(static_cast<const EventType &>(baseEvent));
        };
        m_callbacks[typeid(EventType)][m_keyCounter] = wrapper;
        return m_keyCounter++;
    }

    template<typename EventType>
    void removeEventListener(int key) {
        m_callbacks[typeid(EventType)].erase(key);
    }

    void emit(const BaseEvent &event) {
        auto &eventTypeCallbacks = m_callbacks[typeid(event)];
        for (auto &pair: eventTypeCallbacks) {
            pair.second(event);
        }
    }

    int getNumberOfEventTypes() {
        return static_cast<int>(m_callbacks.count(typeid(BaseEvent)));
    }

    EventBus(const EventBus &) = delete;
    EventBus& operator=(const EventBus &) = delete;
    EventBus(EventBus &&) = delete;
    EventBus& operator=(EventBus &&) = delete;
};