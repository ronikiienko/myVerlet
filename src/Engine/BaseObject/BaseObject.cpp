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
    m_scene.getObjectStorage().toggleObjectRotation(this, enabled);
}
