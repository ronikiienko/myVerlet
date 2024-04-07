#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "IDamageable.h"

class Bullet : public BaseObject {
    void v_onTick() override {}
    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Yellow;
    };
    void v_onCollision(BaseObject *ptr) override {
        if (auto damageable = dynamic_cast<IDamageable*>(ptr)) {
            damageable->receiveDamage(1);
        }
        destroy();
    };
public:
    explicit Bullet() = default;
};