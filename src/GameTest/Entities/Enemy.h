#pragma once

#include "../../Engine/BaseObject.h"
#include "Player.h"

class Enemy : public BaseObject {
private:
    Player *playerPtr;
    float maxAcceleration = 200;
    float calmAcceleration = 20;
    RNGf &m_gen;
    int m_ticksSinceLastRandomDirectionChange = 0;
    int m_ticksToChangeRandomDirection = 100;
    Vector2F m_currentRandomDirection = Vector2F::cart(0, 0);
    float m_viewRadius = 300;
public:
    Enemy(Player *playerPtr, RNGf &gen) : playerPtr(playerPtr), m_gen(gen) {};

    void v_onTick() override {


        if (m_ticksSinceLastRandomDirectionChange >= m_ticksToChangeRandomDirection) {
            m_ticksSinceLastRandomDirectionChange = 0;
            m_currentRandomDirection = Vector2F::cart(m_gen.getInRange(-1, 1), m_gen.getInRange(-1, 1)).normalize();
        } else {
            m_ticksSinceLastRandomDirectionChange++;
        }


        Vector2F acceleration = Vector2F::cart(0, 0);

        Vector2F playerPos = playerPtr->m_basicDetails->m_posCurr;
        float distanceToPlayer = (m_basicDetails->m_posCurr - playerPos).magnitude();
        bool isPlayerInRadius = distanceToPlayer < m_viewRadius;

        if (isPlayerInRadius) {
            Vector2F awayFromPlayer = (m_basicDetails->m_posCurr - playerPos).normalize();
            float speedRatio = 1.0 - std::pow(distanceToPlayer / m_viewRadius, 3);

            acceleration = (m_currentRandomDirection + awayFromPlayer) / 2 * maxAcceleration * speedRatio;
        } else {
            acceleration = m_currentRandomDirection * calmAcceleration;
        }
        m_basicDetails->accelerate(acceleration);
    }

    void v_onCollision(BaseObject *ptr) override {}

    void v_onInit() override {
        m_basicDetails->m_color = sf::Color::Red;
    }
};