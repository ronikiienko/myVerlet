#pragma once

#include "VerletObject.h"

class VerletObjectCollisionHandled : public VerletObject {
    explicit VerletObjectCollisionHandled(Vector2 position) : VerletObject(position) {

    }

    void onCollision() override {
        std::cout << "Collision" << std::endl;
    }
};