#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../../Engine/Scene.h"
#include "Interfaces/IDamageable.h"

class Bullet : public BaseObject {
    int m_ticksToLive = 100;
    int m_hitsLeft = 10;
public:
    explicit Bullet(ObjectContext context) : BaseObject(context) {}
    void v_onInit() override {};
    void v_onCollision(BaseObject *ptr) override {
        if (auto enemy = dynamic_cast<IDamageable *>(ptr)) {
            enemy->damage(1);
        }
        m_hitsLeft--;
        if (m_hitsLeft <= 0) {
            m_scene.removeObject(this);
        }
    };
    void v_onTick() override {
        m_ticksToLive--;
        if (m_ticksToLive <= 0) {
            m_scene.removeObject(this);
        }
    };

    explicit Bullet(ObjectContext context) : BaseObject(context) {};
};