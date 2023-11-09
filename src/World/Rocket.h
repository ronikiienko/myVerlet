#pragma once

#include "ComplexObject.h"

class Rocket : public ComplexObject {
public:
    explicit Rocket(AtomWorld& atomWorld) : ComplexObject(atomWorld) {
        int obj1Ind = atomWorld.addObject(Vector2::fromCartesian(300,300));
        int obj2Ind = atomWorld.addObject(Vector2::fromCartesian(400,400));
        atomWorld.addStick(obj1Ind, obj2Ind);
    }
};