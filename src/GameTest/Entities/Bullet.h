#pragma once

#include "../../Engine/BaseObject.h"
#include "../../Engine/Scene.h"

class Bullet : public BaseObject {
    Scene& m_scene;
    int m_ticksToLive = 100;
public:
    Bullet(Scene& scene) : m_scene(scene) {};
    void v_onInit() override {};
    void v_onCollision(BaseObject *ptr) override {
        m_scene.removeObject(ptr);
        m_scene.removeObject(this);
    };
    void v_onTick() override {
        m_ticksToLive--;
        if (m_ticksToLive <= 0) {
            m_scene.removeObject(this);
        }
    };
};