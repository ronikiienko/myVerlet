#pragma once

#include <thread>
#include "utils/Vector.h"
#include "utils/Rectangle.h"


namespace engineDefaults {
    const int maxObjectNum = 200000;
    constexpr Vector2I worldSize = Vector2I::cart(2400, 1800);
    constexpr Vector2I windowSize = Vector2I::cart(2400, 1800);
    constexpr int objectsRadius = 2;
    constexpr int twoObjectsRadius = objectsRadius + objectsRadius;
    constexpr int twoObjectsRadiusSquared = twoObjectsRadius * twoObjectsRadius;
    // collision grid when finding collisions is split on many threads (by columns). To avoid race conditions i use two pass method. This means that grid in fact will be split on numberOfThreads * 2 tasks. So, if we have 10threads
    // and grid of, say, size 39 will mean that each thread gets 1 task, and 19 remain for "cleanup" function, which is inefficient, because cleanup function (it solves remaining colums) runs on one core
    // Conclusion: best if grid is divided by threads * 2 without remainder
    constexpr int collisionGridWidth = worldSize.x / (objectsRadius * 2);
    constexpr int collisionGridHeight = worldSize.y / (objectsRadius * 2);
    const float physicsInterval = 0.016f;

    constexpr Vector2F gravity = Vector2F::cart(0, 0);
    const int physicsSubSteps = 8;
    const float wallsDamping = 0.8f;
    const float collisionRestitution = 0.45f;
    const float maxVelocity = 4;
    const float linearDamping = 0.9992f;
    const short numThreads = 4;
}

