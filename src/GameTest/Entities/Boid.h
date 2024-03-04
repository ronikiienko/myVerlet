#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../Levels/BoidsBlackboard.h"

class Boid : public BaseObject {
private:
    RNGf &m_gen;
    int m_wanderInterval = 20;
    int m_nextWander = m_wanderInterval;
    BoidsBlackboard& m_blackboard;
public:
    explicit Boid(ObjectContext context, RNGf &gen, BoidsBlackboard& blackboard) : BaseObject(context), m_gen(gen), m_blackboard(blackboard) {}

    Vector2F align() {
        Vector2F desiredVelocity = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, m_blackboard.m_alignRadius, [&](BaseObject *ptr, int id) {
            if (ptr != this) {
                desiredVelocity += ptr->getBasicDetails().getVelocity();
                count++;
            }
        });
        if (count > 0) {
            desiredVelocity /= static_cast<float>(count);
            Vector2F vectorToMakeVelocityDesired = desiredVelocity - getBasicDetails().getVelocity();
            return (vectorToMakeVelocityDesired * 1000).limitMagnitude(1);
        }
        return Vector2F::cart(0, 0);
    }

    Vector2F cohesion() {
        Vector2F centerOfMass = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, m_blackboard.m_cohesionRadius, [&](BaseObject *ptr, int id) {
            if (ptr != this) {
                centerOfMass += ptr->getBasicDetails().m_posCurr;
                count++;
            }
        });
        if (count > 0) {
            centerOfMass /= static_cast<float>(count);
            Vector2F vectorToCenterOfMass = centerOfMass - getBasicDetails().m_posCurr;
            return (vectorToCenterOfMass * 1000).limitMagnitude(1);
        }
        return Vector2F::cart(0, 0);
    }

    Vector2F separation() {
        Vector2F awaySum = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, m_blackboard.m_separationRadius, [&](BaseObject *ptr, int id) {
            if (ptr != this) {
                Vector2F away = (ptr->getBasicDetails().m_posCurr - getBasicDetails().m_posCurr) * -1;
                awaySum += away * (away.magnitude() / m_blackboard.m_separationRadius);
                count++;
            }
        });

        if (count > 0) {
            return awaySum.limitMagnitude(1);
        }
        return Vector2F::cart(0, 0);
    }

    void v_onTick() override {
        Vector2F acceleration =
                separation() * m_blackboard.m_separationWeight + cohesion() * m_blackboard.m_cohesionWeight + align() * m_blackboard.m_alignWeight;
        acceleration = (acceleration * 1000).limitMagnitude(m_blackboard.m_maxAcceleration);
        getBasicDetails().accelerate(acceleration);
//        Vector2F totalMovement = align();
//        getBasicDetails().setVelocity(totalMovement);
    };

    void v_onInit() override {};

    void v_onCollision(BaseObject *ptr) override {};
};