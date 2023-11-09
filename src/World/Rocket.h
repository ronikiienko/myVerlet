#pragma once

#include "ComplexObject.h"

class Rocket : public ComplexObject {
public:
    explicit Rocket(AtomWorld &atomWorld) : ComplexObject(atomWorld) {
        objects.push_back(atomWorld.addObject(Vector2::fromCartesian(300, 300)));
        objects.push_back(atomWorld.addObject(Vector2::fromCartesian(400, 400)));
        sticks.push_back(atomWorld.addStick(objects[0], objects[1]));
    }
};