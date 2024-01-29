#pragma once

#include "../Scene.h"
#include "../EngineConsts.h"

class Shooter {
private:
    float m_speed = 2;
    Scene &m_scene;
public:
    Shooter(Scene &scene) : m_scene(scene) {}
    template<typename BulletType>
    void shoot(Vector2F source, Vector2F target, BulletType &&bullet) {
        Vector2F direction = (target - source).normalize();
        Vector2F velocity = direction * m_speed;
        Vector2F startPos = source + direction * 4;
        std::weak_ptr<BaseObject> ptr = m_scene.addObject(std::forward<BulletType>(bullet), startPos);
        ptr.lock().get()->m_basicDetails->setVelocity(velocity);
    }
};