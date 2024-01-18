#pragma once
#include <typeindex>
#include <memory>
#include <iostream>

class EventBusImpl;
class EBHandle {
private:
    int m_key = -1;
    std::type_index m_type = typeid(int);
    std::weak_ptr<EventBusImpl> m_eventBusPtr;

public:
    EBHandle() = default;
    EBHandle(std::weak_ptr<EventBusImpl> eventBusPtr, std::type_index type, int key);

    void remove();
    bool isOwner();
    ~EBHandle();

    EBHandle (const EBHandle&) = delete;
    EBHandle& operator= (const EBHandle&) = delete;

    EBHandle(EBHandle&&) noexcept;
    EBHandle& operator= (EBHandle&&) noexcept;
};