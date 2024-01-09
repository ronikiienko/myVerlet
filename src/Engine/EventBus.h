#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>

struct BaseEvent {
    BaseEvent() = default;
    virtual ~BaseEvent() = default;
};

class EventBus {
    std::unordered_map<std::type_index, std::unordered_map<int, std::function<void(const BaseEvent &)>>> callbacks;
    int keyCounter = 0;
public:
    template<typename EventType>
    int addEventListener(const std::function<void(const EventType &)> &callback) {
        // here passing callback as copy is necessary. otherwise if parameter is a reference and it is destroyed, lambda will have dangling reference
        auto wrapper = [callback](const BaseEvent &baseEvent) {
            callback(static_cast<const EventType &>(baseEvent));
        };
        callbacks[typeid(EventType)][keyCounter] = wrapper;
        return keyCounter++;
    }

    template<typename EventType>
    void removeEventListener(int key) {
        callbacks[typeid(EventType)].erase(key);
    }

    void emit(const BaseEvent &event) {
        auto &eventTypeCallbacks = callbacks[typeid(event)];
        for (auto &pair: eventTypeCallbacks) {
            pair.second(event);
        }
    }
};