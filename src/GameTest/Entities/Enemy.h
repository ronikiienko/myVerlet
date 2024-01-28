#pragma once

#include "../../Engine/BaseObject.h"
#include "Player.h"

class Enemy : public BaseObject {
private:
    Player* playerPtr;
    float maxAcceleration = 200;
public:
    explicit Enemy(Player* playerPtr) : playerPtr(playerPtr) {};
    void v_onTick() override {
        Vector2F acceleration = Vector2F::cart(0, 0);
        Vector2F playerPos = playerPtr->m_basicDetails->m_posCurr;
        acceleration = (m_basicDetails->m_posCurr - playerPos).normalize() * maxAcceleration;
        m_basicDetails->accelerate(acceleration);
    }
    void v_onCollision(BaseObject *ptr) override {}
    void v_onInit() override {
        m_basicDetails->m_color = sf::Color::Red;
    }
};