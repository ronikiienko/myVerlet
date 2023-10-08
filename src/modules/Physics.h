#pragma once

#include "World.h"
#include "SFML/Graphics/RenderWindow.hpp"

class Physics {
private:
    World &world;

    void applyGravity() {
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            object.accelerate(gravity);
        }
    }

    void updatePositions(float dt) {
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            object.update(dt);
        }
    }

public:
    explicit Physics(World &world) : world(world) {};

    void update() {
        applyGravity();
        updatePositions(physicsInterval);
    }
};