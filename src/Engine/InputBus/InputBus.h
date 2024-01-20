#pragma once

#include "InputBusImpl.h"
#include <memory>

class InputBus {
private:
    std::shared_ptr<InputBusImpl> m_impl;
public:
    explicit InputBus(sf::RenderWindow &window) : m_impl(std::make_shared<InputBusImpl>(window)) {};
    void update(sf::Event& event) {
        m_impl->update(event);
    };

    void clear() {
        m_impl->clear();
    };

    int addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)>& callback) {
        m_impl->addEventListener(type, callback);
    };
};