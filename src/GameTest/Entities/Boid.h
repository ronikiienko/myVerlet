#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../Levels/BoidsBlackboard.h"
#include "Predator.h"
#include "Interfaces/IPrey.h"

class Boid : public BaseObject, public IPrey, public IDamageable {
private:
    RNGf &m_gen;
    int m_wanderInterval = 20;
    int m_nextWander = m_wanderInterval;
    BoidsBlackboard& m_blackboard;

    float m_maxVelocity = -1000000;
public:
    explicit Boid(ObjectContext context, RNGf &gen, BoidsBlackboard& blackboard) : BaseObject(context), m_gen(gen), m_blackboard(blackboard) {}

    Vector2F align() {
        Vector2F velocitySum = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, m_blackboard.m_alignRadius, [&](BaseObject *ptr, int id) {
            if (ptr != this && dynamic_cast<Boid*>(ptr)) {
                velocitySum += ptr->getBasicDetails().getVelocity();
                count++;
            }
        });
        if (count > 0) {
            Vector2F averageVelocity  = velocitySum /= static_cast<float>(count);
            Vector2F vectorToMakeVelocityDesired = velocitySum - getBasicDetails().getVelocity();
            return vectorToMakeVelocityDesired;
        }
        return Vector2F::cart(0, 0);
    }

    Vector2F cohesion() {
        Vector2F centerOfMass = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, m_blackboard.m_cohesionRadius, [&](BaseObject *ptr, int id) {
            if (ptr != this && dynamic_cast<Boid*>(ptr)) {
                centerOfMass += ptr->getBasicDetails().m_posCurr;
                count++;
            }
        });
        if (count > 0) {
            centerOfMass /= static_cast<float>(count);
            Vector2F vectorToCenterOfMass = centerOfMass - getBasicDetails().m_posCurr;
            return (vectorToCenterOfMass * 1000).normalize();
        }
        return Vector2F::cart(0, 0);
    }

    Vector2F separation() {
        Vector2F awaySum = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, m_blackboard.m_separationRadius, [&](BaseObject *ptr, int id) {
            if (ptr != this && dynamic_cast<Boid*>(ptr)) {
                Vector2F away = (ptr->getBasicDetails().m_posCurr - getBasicDetails().m_posCurr) * -1;
                awaySum += away * (away.magnitude() / m_blackboard.m_separationRadius);
                count++;
            }
        });

        if (count > 0) {
            return awaySum.normalize();
        }
        return Vector2F::cart(0, 0);
    }

    Vector2F fear() {
        Vector2F awaySum = Vector2F::cart(0, 0);
        int count = 0;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, 40, [&](BaseObject *ptr, int id) {
            if (ptr != this && dynamic_cast<IPredator*>(ptr)) {
                Vector2F away = (ptr->getBasicDetails().m_posCurr - getBasicDetails().m_posCurr) * -1;
                awaySum += away.normalize();
                count++;
            }
        });

        if (count > 0) {
            return awaySum.normalize();
        }
        return Vector2F::cart(0, 0);
    }

    void v_onTick() override {
        Vector2F acceleration = Vector2F::cart(0, 0);
        Vector2F oldVelocity = getBasicDetails().getVelocity();
        acceleration += align() * m_blackboard.m_alignWeight;
        acceleration += separation() * m_blackboard.m_separationWeight * 0.12;
        acceleration += cohesion() * m_blackboard.m_cohesionWeight * 0.02;
        acceleration += fear() * 1;
        acceleration.limitMagnitudeModify(m_blackboard.m_maxAcceleration);
        getBasicDetails().accelerate(acceleration);
        getBasicDetails().m_direction = getBasicDetails().getVelocity().normalize();
    };

    void v_onInit() override {
        Vector2F direction = Vector2F::cart(m_gen.getInRange(-1, 1), m_gen.getInRange(-1, 1)).normalize();
        getBasicDetails().setVelocity(direction * m_blackboard.m_minVelocity);
        toggleRotation(true);
    };

    void v_onCollision(BaseObject *ptr) override {};

    void receiveDamage(int damage) override {
        destroy();
    };
};