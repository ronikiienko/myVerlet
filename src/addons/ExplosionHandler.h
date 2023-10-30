#pragma once

#include "../World/World.h"

class ExplosionHandler {
private:
    World& world;
public:
    explicit ExplosionHandler(World& world) : world(world) {}

    void launch(Vector2 position, float strength, float radius) {
        world.forEachObject([&position, &strength, &radius](VerletObject& object, int i){
            Vector2 direction = object.posCurr - position;
            float distance = direction.magnitude();
            if (distance <= radius) {
                const float individualStrength = strength * (1 - distance / radius);
                Vector2 addedVelocity = direction / distance * individualStrength;
                object.addVelocity(addedVelocity);
            }
        });
    }
};