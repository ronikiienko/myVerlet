#pragma once

#include "World.h"

class ComplexObject {
public:
    World& world;
    std::vector<int> objects;
    std::vector<int> sticks;

    ComplexObject(World& world) : world(world) {

    }
};