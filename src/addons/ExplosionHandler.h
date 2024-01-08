#pragma once

#include "../Engine/Scene.h"

class ExplosionHandler {
private:
    Scene& scene;
public:
    explicit ExplosionHandler(Scene& scene) : scene(scene) {}

    void launch(Vector2F position, float strength, float radius) {
        scene.forEachBasicDetails([&position, &strength, &radius](BasicDetails& object, int i){
            Vector2F direction = object.posCurr - position;
            float distance = direction.magnitude();
            if (distance <= radius) {
                const float individualStrength = strength * (1 - distance / radius);
                Vector2F addedVelocity = direction / distance * individualStrength;
                // TODO use acceleration instead of setting velocity
                object.addVelocity(addedVelocity);
            }
        });
    }
};