#pragma once


#include "VerletObject.h"

class VerletObject2 : public VerletObject {
public:
    explicit VerletObject2(Vector2 position) : VerletObject(position) {

    }

    void onCollision() override {
        std::cout << "collision\n";
    }
};