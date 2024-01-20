#include "InputBusImpl.h"


IBHandle::IBHandle(std::weak_ptr<InputBusImpl> inputBusPtr, sf::Event::EventType type, int key) : m_inputBusPtr(
        std::move(inputBusPtr)), m_type(type), m_key(key) {}

void IBHandle::remove() {
    if (auto inputBusPtr = m_inputBusPtr.lock()) {
        inputBusPtr->removeEventListener(m_type, m_key);
        m_inputBusPtr = std::shared_ptr<InputBusImpl>();
    }
}

bool IBHandle::isOwner() {
    return !m_inputBusPtr.expired();
}

IBHandle::~IBHandle() {
    remove();
}

IBHandle::IBHandle(IBHandle && other) noexcept {
    m_key = other.m_key;
    m_type = other.m_type;
    m_inputBusPtr = other.m_inputBusPtr;
    other.m_inputBusPtr = std::weak_ptr<InputBusImpl>();
}

IBHandle &IBHandle::operator=(IBHandle && other) noexcept {
    if (this != &other) {
        remove();
        m_key = other.m_key;
        m_type = other.m_type;
        m_inputBusPtr = other.m_inputBusPtr;
        other.m_inputBusPtr = std::weak_ptr<InputBusImpl>();
    }
    return *this;
}
