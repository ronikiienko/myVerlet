#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "Player.h"
#include "Interfaces/IDamageable.h"

class Enemy : public BaseObject, public IDamageable {
private:
    Player *playerPtr;
    float maxAcceleration = 200;
    float calmAcceleration = 20;
    RNGf &m_gen;
    int m_ticksSinceLastRandomDirectionChange = 0;
    int m_ticksToChangeRandomDirection = 100;
    Vector2F m_currentRandomDirection = Vector2F::cart(0, 0);
    float m_viewRadius = 300;

    int m_health = 1;
public:
    Enemy(ObjectContext context, Player *playerPtr, RNGf &gen) : playerPtr(playerPtr), m_gen(gen), BaseObject(context) {};

    void v_onTick() override {


        if (m_ticksSinceLastRandomDirectionChange >= m_ticksToChangeRandomDirection) {
            m_ticksSinceLastRandomDirectionChange = 0;
            m_currentRandomDirection = Vector2F::cart(m_gen.getInRange(-1, 1), m_gen.getInRange(-1, 1)).normalize();
        } else {
            m_ticksSinceLastRandomDirectionChange++;
        }


        Vector2F acceleration = Vector2F::cart(0, 0);

        Vector2F playerPos = playerPtr->getBasicDetails().m_posCurr;
        float distanceToPlayer = (getBasicDetails().m_posCurr - playerPos).magnitude();
        bool isPlayerInRadius = distanceToPlayer < m_viewRadius;

        if (isPlayerInRadius) {
            Vector2F awayFromPlayer = (getBasicDetails().m_posCurr - playerPos).normalize();
            float speedRatio = 1.0 - std::pow(distanceToPlayer / m_viewRadius, 3);

            acceleration = (m_currentRandomDirection + awayFromPlayer) / 2 * maxAcceleration * speedRatio;
        } else {
            acceleration = m_currentRandomDirection * calmAcceleration;
        }
        getBasicDetails().accelerate(acceleration);
    }

    void v_onCollision(BaseObject *ptr) override {}

    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Red;
    }

    void receiveDamage(int damage) override {
        m_health -= damage;
        if (m_health <= 0) {
            destroy();
        }
    }
};