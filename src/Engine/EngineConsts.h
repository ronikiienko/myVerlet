#pragma once

#include <thread>
#include "utils/Vector.h"
#include "utils/Rectangle.h"


namespace engineDefaults {
    constexpr Vector2I windowSize = Vector2I::cart(2400, 1000);
    constexpr int objectsRadius = 2;
    constexpr int twoObjectsRadius = objectsRadius + objectsRadius;
    constexpr int twoObjectsRadiusSquared = twoObjectsRadius * twoObjectsRadius;

    constexpr Vector2I worldSize = Vector2I::cart(2400, 1800);
    const int maxObjectsNum = 200000;
    constexpr Vector2F gravity = Vector2F::cart(0, 0);
    const int physicsSubSteps = 8;
    const float wallsDamping = 0.8f;
    const float collisionRestitution = 0.45f;
    const float maxVelocity = 4;
    const float linearDamping = 0.9992f;
    const short numThreads = 4;
    // actually, only thing it does is affecting how much given m_acceleration affects velocity
    // To make it affect more, increase this value
    const float physicsInterval = 0.001f;

    float cameraLongestDimViewSize = 400;
    Vector2F cameraDefaultPosition = Vector2F::cart(0, 0);
}

