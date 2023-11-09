#pragma once

#include "ComplexObject.h"

class Dumb : public ComplexObject {
public:
    explicit Dumb(AtomWorld& atomWorld, Vector2 initPos) : ComplexObject(atomWorld) {
        objects.push_back(atomWorld.addObject(initPos));
    }

    void update() override {
        VerletObject& object = atomWorld.getObject(objects[0]);
        object.addVelocity(Vector2::fromCartesian(0.01,0.01));
    }
};