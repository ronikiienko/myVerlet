#pragma once

#include <thread>
#include "utils/Vector.h"
#include "utils/Rectangle.h"


namespace consts {
    const int maxObjectNum = 200000;
    constexpr Vector2 gravity = Vector2::cart(0, 0);
    constexpr RectangleI worldBounds = RectangleI::fromSize(0, 0, 2400, 1800);
    constexpr RectangleI windowBounds = RectangleI::fromSize(0, 0, 2400, 1800);
    constexpr int objectsRadius = 2;
    constexpr int twoObjectsRadius = objectsRadius + objectsRadius;
    constexpr int twoObjectsRadiusSquared = twoObjectsRadius * twoObjectsRadius;
    // collision grid when finding collisions is split on many threads (by columns). To avoid race conditions i use two pass method. This means that grid in fact will be split on numberOfThreads * 2 tasks. So, if we have 10threads
    // and grid of, say, size 39 will mean that each thread gets 1 task, and 19 remain for "cleanup" function, which is inefficient, because cleanup function (it solves remaining colums) runs on one core
    // Conclusion: best if grid is divided by threads * 2 without remainder
    constexpr int collisionGridWidth = worldBounds.getWidth() / (objectsRadius * 2);
    constexpr int collisionGridHeight = worldBounds.getHeight() / (objectsRadius * 2);
    const float physicsInterval = 0.016f;
    const int physicsSubSteps = 8;
    // how much speed is damped when hitting wall
    const float wallsDamping = 0.8f;
    // doesn't actually change speed when collision happens, but rather how much objects will be "splitted" when resolving collisions. 0 - not splitted no collision resolving happens. 1 - objects are fully splitted
    const float collisionRestitution = 0.45f;
    // we limit velocity of each object on each update() call. This can prevent full chaos.
    // changes chains behaviours
    const float maxVelocity = 4;
    // we just multiply velocity of each object by this value on each substep. Can help a bit if simulation goes to chaos
    const float linearDamping = 0.9992f;
    // when same - whole simulation goes same way each time you launch it
    const int seed = 40;
    const short numThreads = 4;
}

