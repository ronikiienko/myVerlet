#pragma once

#include <utility>

#include "../../Engine/BaseObject/BaseObject.h"
#include "../../Engine/InputBus/InputBus.h"
#include "IDamageable.h"

class Enemy : public BaseObject, public IDamageable {
private:
    std::weak_ptr<Player> m_playerPtr;
    RNGf& m_gen;

    float m_viewDistance = m_gen.getInRange(50, 150);
    float m_maxAcceleration = 0.1;
    float m_health = 100;

    EventBus& m_eventBus;
public:
    explicit Enemy(ObjectContext context, std::weak_ptr<Player> playerPtr, RNGf& gen, EventBus& eventBus) : BaseObject(context), m_playerPtr(std::move(playerPtr)), m_gen(gen), m_eventBus(eventBus) {}

    void v_onTick() override {
        auto player = m_playerPtr.lock();
        if (player){
            float distance = (getBasicDetails().m_posCurr - player->getBasicDetails().m_posCurr).magnitude();
            if (distance < m_viewDistance) {
                getBasicDetails().accelerate((getBasicDetails().m_posCurr - player->getBasicDetails().m_posCurr).normalize() * 0.08);
            }
        }
        Vector2F wallAvoidance = Vector2F::cart();

        float toLeft = m_scene.getDistanceToLeft(getBasicDetails().m_posCurr);
        float toRight = m_scene.getDistanceToRight(getBasicDetails().m_posCurr);
        float toTop = m_scene.getDistanceToTop(getBasicDetails().m_posCurr);
        float toBottom = m_scene.getDistanceToBottom(getBasicDetails().m_posCurr);


        if (toLeft < m_viewDistance) {
            wallAvoidance.m_x += 0.1f * ((m_viewDistance - toLeft) / m_viewDistance);
        }
        if (toRight < m_viewDistance) {
            wallAvoidance.m_x -= 0.1f * ((m_viewDistance - toRight) / m_viewDistance);
        }
        if (toTop < m_viewDistance) {
            wallAvoidance.m_y += 0.1f * ((m_viewDistance - toTop) / m_viewDistance);
        }
        if (toBottom < m_viewDistance) {
            wallAvoidance.m_y -= 0.1f * ((m_viewDistance - toBottom) / m_viewDistance);
        }
        getBasicDetails().accelerate(wallAvoidance);
    };
    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Red;
    };
    void v_onCollision(BaseObject* ptr) override {};

    void receiveDamage(float damage) override {
        destroy();
    }
};