#include "BaseObject.h"
#include "../Scene/Scene.h"
//
// Created by ronik on 29/01/2024.
//
void BaseObject::destroy() {
    m_scene.getObjectStorage().removeObject(this);
}

BaseObject::BaseObject(ObjectContext objectContext) : m_scene(objectContext.m_scene) {
}

void BaseObject::toggleRotation(bool enabled) {
#ifdef IT_IS_DEBUG
    if (m_id == -1) {
        throw std::runtime_error("Id is not set in BaseObject.");
    }
#endif
    m_scene.getObjectStorage().toggleObjectRotation(m_id, enabled);
}
