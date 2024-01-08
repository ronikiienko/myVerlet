#pragma once

#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class InputHandler {
private:
    sf::RenderWindow &window;
    std::unordered_map<sf::Event::EventType, std::unordered_map<int, std::function<void(const sf::Event &)>>> eventHandlers;
    int keyCounter = 0;
public:
    explicit InputHandler(sf::RenderWindow &window) : window(window) {}
    void update() {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else {
                auto &eventTypeCallbacks = eventHandlers[event.type];
                for (auto &pair: eventTypeCallbacks) {
                    std::cout << "Joystick: " << event.joystickButton.joystickId << std::endl;
                    pair.second(event);
                }
            }
        }
    }


    int addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)>& callback) {
        eventHandlers[type][keyCounter] = callback;
        keyCounter++;
        return keyCounter - 1;
    }


    void removeEventListener(sf::Event::EventType type, int key) {
        eventHandlers[type].erase(key);
    }
};