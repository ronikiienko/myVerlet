#pragma once

#include "../Scene.h"

class ExplosionHandler {
private:
    Scene& m_scene;
public:
    explicit ExplosionHandler(Scene& scene) : m_scene(scene) {}

    void launch(Vector2F position, float strength, float radius) {
        m_scene.forEachBasicDetails([&position, &strength, &radius](BasicDetails& object, int i){
            Vector2F direction = object.m_posCurr - position;
            float distance = direction.magnitude();
            if (distance <= radius) {
                const float individualStrength = strength * (1 - distance / radius);
                Vector2F addedVelocity = direction / distance * individualStrength;
                // TODO use m_acceleration instead of setting velocity
                object.addVelocity(addedVelocity);
            }
        });
    }
};