#pragma once

#include "../AtomWorld/AtomWorld.h"

class ComplexObject {
public:
    AtomWorld& atomWorld;
    std::vector<int> objects;
    std::vector<int> sticks;

    explicit ComplexObject(AtomWorld& atomWorld) : atomWorld(atomWorld) {};

    virtual void update() {};
};


