//
// Created by ronik on 29/01/2024.
//
#include "SMHandle.h"

#include <utility>
#include "SoundManagerImpl.h"

SMHandle::SMHandle(std::weak_ptr<SoundManagerImpl> soundManagerPtr, int key) : m_soundManagerPtr(std::move(soundManagerPtr)), m_key(key) {

}

void SMHandle::remove() {
    if (auto soundManagerPtr = m_soundManagerPtr.lock()) {
        soundManagerPtr->remove(m_key);
        m_soundManagerPtr = std::shared_ptr<SoundManagerImpl>();
    }
}

bool SMHandle::isOwner() {
    return !m_soundManagerPtr.expired();
}

SMHandle::~SMHandle() {
    remove();
}

SMHandle::SMHandle(SMHandle && other) noexcept {
    m_key = other.m_key;
    m_soundManagerPtr = other.m_soundManagerPtr;
    other.m_soundManagerPtr = std::weak_ptr<SoundManagerImpl>();
}

SMHandle &SMHandle::operator=(SMHandle && other) noexcept {
    if (this != &other) {
        remove();
        m_key = other.m_key;
        m_soundManagerPtr = other.m_soundManagerPtr;
        other.m_soundManagerPtr = std::weak_ptr<SoundManagerImpl>();
    }
    return *this;
}

