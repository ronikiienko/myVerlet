#pragma once

#include <SFML/Window/Event.hpp>
#include <memory>
#include "InputBusImpl.h"

class InputBusImpl;
class IBHandle {
private:
    int m_key = -1;
    sf::Event::EventType m_type = sf::Event::JoystickButtonPressed;
    std::weak_ptr<InputBusImpl> m_inputBusPtr;
public:
    IBHandle() = default;
    IBHandle(std::weak_ptr<InputBusImpl> inputBusPtr, sf::Event::EventType type, int key);

    void remove();
    bool isOwner();
    ~IBHandle();

    IBHandle (const IBHandle&) = delete;
    IBHandle& operator= (const IBHandle&) = delete;

    IBHandle(IBHandle&&) noexcept;
    IBHandle& operator= (IBHandle&&) noexcept;
};