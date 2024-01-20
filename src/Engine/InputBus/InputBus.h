#pragma once

#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class InputBus {
private:
    sf::RenderWindow &m_window;
    std::unordered_map<sf::Event::EventType, std::unordered_map<int, std::function<void(const sf::Event &)>>> m_eventHandlers;
    int m_keyCounter = 0;
public:
    explicit InputBus(sf::RenderWindow &window);
    void update(sf::Event& event);

    void clear();

    int addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)>& callback);

    void removeEventListener(sf::Event::EventType type, int key);

    InputBus(const InputBus &) = delete;
    InputBus& operator=(const InputBus &) = delete;
    InputBus(InputBus &&) = delete;
    InputBus& operator=(InputBus &&) = delete;
};