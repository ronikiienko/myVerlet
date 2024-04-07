#include "BaseObject.h"
#include "../Scene/Scene.h"

//
// Created by ronik on 29/01/2024.
//
void BaseObject::destroy() const {
#ifdef IT_IS_DEBUG
    if (m_id == -1 || m_scene == nullptr) {
        throw std::runtime_error("Trying to destroy object that has index or scene uninitialized.");
    }
#endif
    m_scene->getObjectStorage().removeObject(m_id);
}

void BaseObject::toggleRotation(bool enabled) {
#ifdef IT_IS_DEBUG
    if (m_id == -1 || m_scene == nullptr) {
        throw std::runtime_error("Id is not set in BaseObject.");
    }
#endif
    m_scene->getObjectStorage().toggleObjectRotation(m_id, enabled);
}
