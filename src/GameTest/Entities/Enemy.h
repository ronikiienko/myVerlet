#pragma once

#include <utility>

#include "../../Engine/BaseObject/BaseObject.h"
#include "../../Engine/InputBus/InputBus.h"
#include "IDamageable.h"
#include "Food.h"

std::vector<float> getNormalizedWeights(std::vector<float> weights) {
    float sum = 0;
    for (float weight: weights) {
        sum += weight;
    }
    for (float &weight: weights) {
        weight /= sum;
    }
    return weights;
}

struct BehaviourResult {
    Vector2F m_direction;
    float localWeight;
    float m_baseWeight;

    BehaviourResult(Vector2F direction, float weight, float baseWeight) : m_direction(direction),
                                                                          localWeight(weight),
                                                                          m_baseWeight(baseWeight) {}
};

class Enemy : public BaseObject, public IDamageable {
private:
    std::weak_ptr<Player> m_playerPtr;
    RNGf &m_gen;

    float m_viewDistance = 100;
    float m_maxAcceleration = 0.2;
    float bulletAvoidanceDistance = 30;
    float m_health = 100;

    EventBus &m_eventBus;
public:
    explicit Enemy(ObjectContext context, std::weak_ptr<Player> playerPtr, RNGf &gen, EventBus &eventBus) : BaseObject(
            context), m_playerPtr(std::move(playerPtr)), m_gen(gen), m_eventBus(eventBus) {}

    BehaviourResult wallAvoidance() {
        float weight;
        Vector2F wallAvoidanceDirection = Vector2F::cart();

        float toLeft = m_scene.getDistanceToLeft(getBasicDetails().m_posCurr);
        float toRight = m_scene.getDistanceToRight(getBasicDetails().m_posCurr);
        float toTop = m_scene.getDistanceToTop(getBasicDetails().m_posCurr);
        float toBottom = m_scene.getDistanceToBottom(getBasicDetails().m_posCurr);

        float xWeight = 0;
        float yWeight = 0;
        if (toLeft < m_viewDistance) {
            xWeight = (m_viewDistance - toLeft) / m_viewDistance;
            wallAvoidanceDirection.m_x += 1 * xWeight;
        } else if (toRight < m_viewDistance) {
            xWeight = (m_viewDistance - toRight) / m_viewDistance;
            wallAvoidanceDirection.m_x -= 1 * xWeight;
        }
        if (toTop < m_viewDistance) {
            yWeight = (m_viewDistance - toTop) / m_viewDistance;
            wallAvoidanceDirection.m_y += 1 * yWeight;
        } else if (toBottom < m_viewDistance) {
            yWeight = (m_viewDistance - toBottom) / m_viewDistance;
            wallAvoidanceDirection.m_y -= 1 * yWeight;
        }
        weight = std::max(xWeight, yWeight);

        wallAvoidanceDirection = wallAvoidanceDirection.normalize();
        return {wallAvoidanceDirection, weight, 0.5};
    }

    BehaviourResult playerAvoidance() {
        auto player = m_playerPtr.lock();
        float weight = 0;
        Vector2F awayFromPlayerDirection = Vector2F::cart();
        if (player) {
            float distance = (getBasicDetails().m_posCurr - player->getBasicDetails().m_posCurr).magnitude();
            if (distance < m_viewDistance) {
                weight = ((m_viewDistance - distance) / m_viewDistance);
                awayFromPlayerDirection = (getBasicDetails().m_posCurr -
                                           player->getBasicDetails().m_posCurr).normalize();
            }
        }
        return {awayFromPlayerDirection, weight, 0.5};
    }

    BehaviourResult randomMovement() {
        if (m_gen.getInRange(1, 30) > 1) {
            Vector2F direction = Vector2F::polar(1, Angle::fromDegrees(m_gen.getInRange(0, 360)));
            return {direction, 1, 1};
        } else {
            return {Vector2F::cart(), 0, 0.01};
        }

    }

    BehaviourResult bulletAvoidance() {
        Vector2F direction = Vector2F::cart();
        int bulletCount = 0;
        m_scene.getObjectStorage().forEachObjectOfType<Bullet>([&](Bullet &bullet, int id) {
            if ((bullet.getBasicDetails().m_posCurr - getBasicDetails().m_posCurr).magnitude2() <
                bulletAvoidanceDistance * bulletAvoidanceDistance) {
                bulletCount++;
                direction += (getBasicDetails().m_posCurr - bullet.getBasicDetails().m_posCurr);
            }
        });
        if (bulletCount > 0) {
            direction = direction.normalize();
            return {direction, 1, 0.8};
        } else {
            return {direction, 0, 1};
        }
    }

    BehaviourResult foodSearching() {
        float nearestDistance = -10;
        Food *nearestFood = nullptr;
        m_scene.forEachInRadius(getBasicDetails().m_posCurr, 32, [&](BaseObject *object, int id) {
            if (auto food = dynamic_cast<Food *>(object)) {
                float distance = (food->getBasicDetails().m_posCurr - getBasicDetails().m_posCurr).magnitude2();
                if (distance > nearestDistance) {
                    nearestFood = food;
                }
            }
        });
        if (nearestFood) {
            Vector2F direction = (nearestFood->getBasicDetails().m_posCurr - getBasicDetails().m_posCurr).normalize();
            return {direction, 1, 0.3};
        } else {
            return {Vector2F(), 0, 0};
        }
    }

    void v_onTick() override {
        Vector2F totalAcceleration = Vector2F::cart();

        std::vector<BehaviourResult> behaviourResults;

//        behaviourResults.push_back(playerAvoidance());
//        behaviourResults.push_back(wallAvoidance());
        behaviourResults.push_back(randomMovement());
//        behaviourResults.push_back(bulletAvoidance());
//        behaviourResults.push_back(foodSearching());

        std::vector<Vector2F> directions(behaviourResults.size());
        std::vector<float> localWeights(behaviourResults.size());
        std::vector<float> baseWeights(behaviourResults.size());

        for (int i = 0; i < behaviourResults.size(); i++) {
            directions[i] = behaviourResults[i].m_direction;
        }
        for (int i = 0; i < behaviourResults.size(); i++) {
            localWeights[i] = behaviourResults[i].localWeight;
        }
        for (int i = 0; i < behaviourResults.size(); i++) {
            baseWeights[i] = behaviourResults[i].m_baseWeight;
        }

        float maximumLocalWeight = *std::max_element(localWeights.begin(), localWeights.end());

        std::vector<float> normalizedLocalWeights = getNormalizedWeights(localWeights);
        std::vector<float> normalizedBaseWeights = getNormalizedWeights(baseWeights);

        for (int i = 0; i < directions.size(); i++) {
            if (localWeights[i] > 0) {
                totalAcceleration += directions[i] * normalizedLocalWeights[i] * normalizedBaseWeights[i];
            }
        }

        if (totalAcceleration.magnitude2() > 0) {
            totalAcceleration = totalAcceleration.normalize() * m_maxAcceleration * maximumLocalWeight;
        }
        getBasicDetails().accelerate(totalAcceleration);
    };

    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Red;
        getBasicDetails().setVelocity(Vector2F::cart(-2, 0));
    };

    void v_onCollision(BaseObject *ptr) override {
        if (auto food = dynamic_cast<Food *>(ptr)) {
            m_health += 10;
            food->receiveDamage(1);
        }
    };

    void receiveDamage(float damage) override {
        destroy();
    }
};