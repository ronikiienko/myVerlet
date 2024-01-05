#pragma once

#include <functional>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class InputHandler {
private:
    sf::RenderWindow &window;
    std::unordered_map<sf::Event::EventType, std::vector<std::function<void(sf::Event &)>>> eventHandlers;
public:
    explicit InputHandler(sf::RenderWindow &window) : window(window) {}
    void update() {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else {
                auto &eventTypeCallbacks = eventHandlers[event.type];
                for (auto &callback: eventTypeCallbacks) {
                    callback(event);
                }
            }
        }
    }

    template<typename T>
    int addEventListener(sf::Event::EventType type, T && callback) {
        eventHandlers[type].push_back(callback);
        return static_cast<int>(eventHandlers[type].size()) - 1;
    }

    void removeEventListener(sf::Event::EventType type, int handle) {
        eventHandlers[type].erase(eventHandlers[type].begin() + handle);
    }
};