#pragma once


#include "../../Engine/Scene/BaseObject/BaseObject.h"
#include "./IDamageable.h"
class Food : public BaseObject, public IDamageable {
public:
    explicit Food()= default;

    void v_onTick() override {};
    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Magenta;
        getBasicDetails().m_mass = 10;
//        getBasicDetails().pin();
    };
    void v_onCollision(BaseObject* ptr) override {};

    void receiveDamage(float damage) override {
        destroy();
    }
};