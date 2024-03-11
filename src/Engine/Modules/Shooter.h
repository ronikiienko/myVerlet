#pragma once

#include "../Scene.h"
#include "../EngineConsts.h"
#include "../utils/Rand.h"


class Shooter {
private:
    RNGf& m_gen;
    float m_speed = 2;
    float m_spreadDegrees = 0;
    Scene &m_scene;
    int m_cooldown = 5;
    int m_ticksSinceLastShot = 0;

    template<typename BulletType>
    void shoot(Vector2F source, Vector2F target, BulletType &&bullet) {
        Angle spreadAngle = Angle::fromDegrees(m_gen.getInRange(-m_spreadDegrees, m_spreadDegrees));
        Vector2F direction = (target - source).rotate(spreadAngle).normalize();
        Vector2F velocity = direction * m_speed;
        Vector2F startPos = source + direction * engineDefaults::objectsRadius * 2;
        std::weak_ptr<BaseObject> ptr = m_scene.addObject(std::forward<BulletType>(bullet), startPos);
        ptr.lock()->getBasicDetails().setVelocity(velocity);
    }
public:
    Shooter(Scene &scene, RNGf& gen) : m_scene(scene), m_gen(gen) {}


    void setSpread(Angle spread) {
        m_spreadDegrees = spread.getDegrees();
    }

    void setSpeed(float speed) {
        m_speed = speed;
    }

    void setCooldown(int cooldown) {
        m_cooldown = cooldown;
    }

    void tick() {
        m_ticksSinceLastShot++;
    }

    template<typename BulletType>
    bool tryShoot(Vector2F source, Vector2F target, BulletType &&bullet) {
        if (m_ticksSinceLastShot > m_cooldown) {
            shoot(source, target, std::forward<BulletType>(bullet));
            m_ticksSinceLastShot = 0;
            return true;
        } else {
            return false;
        }
    }
};