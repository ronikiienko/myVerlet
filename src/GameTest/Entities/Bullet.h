#pragma once

#include "../../Engine/BaseObject.h"
#include "../../Engine/Scene.h"

class Bullet : public BaseObject {
    Scene& m_scene;
public:
    Bullet(Scene& scene) : m_scene(scene) {};
    void v_onInit() override {};
    void v_onCollision(BaseObject *ptr) override {
        m_scene.removeObject(ptr);
    };
    void v_onTick() override {};
};