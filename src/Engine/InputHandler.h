#pragma once

#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class InputHandler {
private:
    sf::RenderWindow &m_window;
    std::unordered_map<sf::Event::EventType, std::unordered_map<int, std::function<void(const sf::Event &)>>> m_eventHandlers;
    int m_keyCounter = 0;
public:
    explicit InputHandler(sf::RenderWindow &window) : m_window(window) {}
    void update(sf::Event& event) {
//        sf::Event event{};
//        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
            } else {
                auto &eventTypeCallbacks = m_eventHandlers[event.type];
                for (auto &pair: eventTypeCallbacks) {
                    pair.second(event);
                }
            }
//        }
    }

    void clear() {
        m_eventHandlers.clear();
    }

    int addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)>& callback) {
        m_eventHandlers[type][m_keyCounter] = callback;
        return m_keyCounter++;
    }


    void removeEventListener(sf::Event::EventType type, int key) {
        m_eventHandlers[type].erase(key);
    }

    InputHandler(const InputHandler &) = delete;
    InputHandler& operator=(const InputHandler &) = delete;
    InputHandler(InputHandler &&) = delete;
    InputHandler& operator=(InputHandler &&) = delete;
};