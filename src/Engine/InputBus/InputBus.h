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

    template<typename T>
    [[nodiscard]] IBHandle addEventListener(sf::Event::EventType type, T&& callback) {
        return m_impl->addEventListener(type, std::forward<T>(callback));
    };
};