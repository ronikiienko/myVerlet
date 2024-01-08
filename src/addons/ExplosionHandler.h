#pragma once

#include "../Engine/AtomWorld.h"

class ExplosionHandler {
private:
    AtomWorld& atomWorld;
public:
    explicit ExplosionHandler(AtomWorld& atomWorld) : atomWorld(atomWorld) {}

    void launch(Vector2F position, float strength, float radius) {
        atomWorld.forEachBasicDetails([&position, &strength, &radius](BasicDetails& object, int i){
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