#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include "EBHandle.h"
#include "BaseEvent.h"
#include <memory>


class EventBusImpl : public std::enable_shared_from_this<EventBusImpl> {
    std::unordered_map<std::type_index, std::unordered_map<int, std::function<void(const BaseEvent &)>>> m_callbacks;
    int m_keyCounter = 0;
public:
    EventBusImpl() = default;

    template<typename EventType>
    [[nodiscard]] EBHandle addEventListener(const std::function<void(const EventType &)> &callback) {
        // passing m_callback by copy is necessary. otherwise if callback passed to addEventListener by reference and it is destroyed, lambda will have dangling reference
        auto wrapper = [callback](const BaseEvent &baseEvent) {
            callback(static_cast<const EventType &>(baseEvent));
        };
        std::type_index type = typeid(EventType);
        m_callbacks[type][m_keyCounter] = wrapper;
        return {weak_from_this(), type, m_keyCounter++};
    };

    template<typename EventType>
    void removeEventListener(int key) {
        m_callbacks[typeid(EventType)].erase(key);
    };

    void removeEventListener(std::type_index type, int key) {
        m_callbacks[type].erase(key);
    };

    void emit(const BaseEvent &event) {
        auto &eventTypeCallbacks = m_callbacks[typeid(event)];
        for (auto &pair: eventTypeCallbacks) {
            pair.second(event);
        }
    };
};