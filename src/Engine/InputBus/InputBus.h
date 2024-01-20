#pragma once

#include "InputBusImpl.h"

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

    IBHandle addEventListener(sf::Event::EventType type, const std::function<void(const sf::Event &)>& callback) {
        return m_impl->addEventListener(type, callback);
    };
};