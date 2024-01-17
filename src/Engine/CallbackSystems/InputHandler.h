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
    explicit InputHandler(sf::RenderWindow &window);
    void update(sf::Event& event);

    void clear();

    int addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)>& callback);

    void removeEventListener(sf::Event::EventType type, int key);

    InputHandler(const InputHandler &) = delete;
    InputHandler& operator=(const InputHandler &) = delete;
    InputHandler(InputHandler &&) = delete;
    InputHandler& operator=(InputHandler &&) = delete;
};