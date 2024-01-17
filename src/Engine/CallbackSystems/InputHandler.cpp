#include "InputHandler.h"

InputHandler::InputHandler(sf::RenderWindow &window) : m_window(window) {}

void InputHandler::clear() {
    m_eventHandlers.clear();
}

int InputHandler::addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)> &callback) {
    m_eventHandlers[type][m_keyCounter] = callback;
    return m_keyCounter++;
}

void InputHandler::removeEventListener(sf::Event::EventType type, int key) {
    m_eventHandlers[type].erase(key);
}

void InputHandler::update(sf::Event &event) {
    if (event.type == sf::Event::Closed) {
        m_window.close();
    } else {
        auto &eventTypeCallbacks = m_eventHandlers[event.type];
        for (auto &pair: eventTypeCallbacks) {
            pair.second(event);
        }
    }
}
