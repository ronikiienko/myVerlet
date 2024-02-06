#pragma once

#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include "IBHandle.h"

class InputBusImpl : public std::enable_shared_from_this<InputBusImpl> {
private:
    sf::RenderWindow &m_window;
    std::unordered_map<sf::Event::EventType, std::unordered_map<int, std::function<void(const sf::Event &)>>> m_eventHandlers;
    int m_keyCounter = 0;
public:
    explicit InputBusImpl(sf::RenderWindow &window) : m_window(window) {};
    void update(sf::Event& event) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        } else {
            auto &eventTypeCallbacks = m_eventHandlers[event.type];
            for (auto &pair: eventTypeCallbacks) {
                pair.second(event);
            }
        }
    };

    void clear() {
        m_eventHandlers.clear();
    };

    template<typename T>
    IBHandle addEventListener(sf::Event::EventType type, T&& callback) {
        m_eventHandlers[type][m_keyCounter] = std::forward<T>(callback);
        return {weak_from_this(), type, m_keyCounter++};
    };

    void removeEventListener(sf::Event::EventType type, int key) {
        m_eventHandlers[type].erase(key);
    };

    InputBusImpl(const InputBusImpl &) = delete;
    InputBusImpl& operator=(const InputBusImpl &) = delete;
    InputBusImpl(InputBusImpl &&) = delete;
    InputBusImpl& operator=(InputBusImpl &&) = delete;
};