#pragma once

#include "ComplexObject.h"

class Rocket : ComplexObject {
public:
    explicit Rocket(AtomWorld& atomWorld) : ComplexObject(atomWorld) {}
};