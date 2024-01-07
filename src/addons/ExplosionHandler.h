#pragma once

#include "../Engine/AtomWorld.h"

class ExplosionHandler {
private:
    AtomWorld& atomWorld;
public:
    explicit ExplosionHandler(AtomWorld& atomWorld) : atomWorld(atomWorld) {}

    void launch(Vector2 position, float strength, float radius) {
        atomWorld.forEachBasicDetails([&position, &strength, &radius](BasicDetails& object, int i){
            Vector2 direction = object.posCurr - position;
            float distance = direction.magnitude();
            if (distance <= radius) {
                const float individualStrength = strength * (1 - distance / radius);
                Vector2 addedVelocity = direction / distance * individualStrength;
                // TODO use acceleration instead of setting velocity
                object.addVelocity(addedVelocity);
            }
        });
    }
};