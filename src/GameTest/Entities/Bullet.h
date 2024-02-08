#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../../Engine/Scene.h"
#include "Interfaces/IDamageable.h"

class Bullet : public BaseObject {
    int m_ticksToLive = 100;
    int m_hitsLeft = 10;
public:
    void v_onInit() override {};
    void v_onTick() override {
        m_ticksToLive--;
        if (m_ticksToLive <= 0) {
            m_scene.removeObject(this);
        }
    };
};