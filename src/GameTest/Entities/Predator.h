#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../Levels/BoidsBlackboard.h"
#include "../../Engine/utils/Rand.h"
#include "Interfaces/IPredator.h"
#include "Interfaces/IPrey.h"

class Predator : public BaseObject, public IPredator {
private:
    RNGf &m_gen;
    int m_wanderInterval = 20;
    int m_nextWander = m_wanderInterval;
    BoidsBlackboard& m_blackboard;

    float m_maxVelocity = -1000000;
    int m_hopInterval = 90;
    int m_ticksToHop = 0;
public:
    explicit Predator(ObjectContext context, RNGf &gen, BoidsBlackboard& blackboard) : BaseObject(context), m_gen(gen), m_blackboard(blackboard) {}



    void v_onTick() override {
        if (m_ticksToHop <= 0) {
            m_ticksToHop = m_hopInterval;

            int count = 0;
            m_scene.forEachInRadius(getBasicDetails().m_posCurr, 60, [&](BaseObject *ptr, int id) {
                if (ptr != this) {
                    auto isPrey = dynamic_cast<IPrey*>(ptr);
                    if (isPrey) {
                        Vector2F toPrey = (ptr->getBasicDetails().m_posCurr - getBasicDetails().m_posCurr);
                        getBasicDetails().accelerate(toPrey / 2);
                        count++;
                    }

                }
            });
        } else {
            getBasicDetails().setVelocity(getBasicDetails().getVelocity() * 0.98);
            m_ticksToHop--;
        }
    };

    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Red;
    };

    void v_onCollision(BaseObject *ptr) override {
        if (auto boid = dynamic_cast<IPrey*>(ptr)) {
            if (auto damageable = dynamic_cast<IDamageable*>(ptr)) {
                damageable->receiveDamage(1);
            }
        }
    };
};