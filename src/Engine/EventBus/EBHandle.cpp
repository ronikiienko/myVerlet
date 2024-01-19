//#include "EBHandle.h" //this is not needed. The program will work without this because EventBusImpl.h includes EBHandle.h
#include "EventBusImpl.h"
#include "EBHandle.h"


EBHandle::EBHandle(std::weak_ptr<EventBusImpl> eventBusPtr, std::type_index type, int key) : m_key(key), m_type(type),
                                                                                             m_eventBusPtr(std::move(
                                                                                                     eventBusPtr)) {
};

void EBHandle::remove() {
    if (auto eventBusPtr = m_eventBusPtr.lock()) {
        eventBusPtr->removeEventListener(m_type, m_key);
        m_eventBusPtr = std::shared_ptr<EventBusImpl>();
    }
}

EBHandle::~EBHandle() {
    remove();
}

EBHandle &EBHandle::operator=(EBHandle &&other) noexcept {
    if (this != &other) {
        remove();
        m_key = other.m_key;
        m_type = other.m_type;
        m_eventBusPtr = other.m_eventBusPtr;
        other.m_eventBusPtr = std::weak_ptr<EventBusImpl>();
    }
    return *this;
}

EBHandle::EBHandle(EBHandle &&other) noexcept {
    m_key = other.m_key;
    m_type = other.m_type;
    m_eventBusPtr = other.m_eventBusPtr;
    other.m_eventBusPtr = std::weak_ptr<EventBusImpl>();
}

bool EBHandle::isOwner() {
    return !m_eventBusPtr.expired();
}
