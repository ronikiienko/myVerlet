#include "BaseObject.h"
#include "../Scene.h"
//
// Created by ronik on 29/01/2024.
//
BasicDetails &BaseObject::getBasicDetails() const {
//        if (m_scene == nullptr) throw std::runtime_error("Getting scene but it's not initialized yet. Call this method only after or inside onInit()");
    return *m_basicDetails;
}

Scene &BaseObject::getScene() const {
    if (m_scene == nullptr) throw std::runtime_error("Getting scene but it's not initialized yet. Call this method only after or inside onInit()");
    return *m_scene;
}

void BaseObject::destroy() {
    getScene().removeObject(this);
}
