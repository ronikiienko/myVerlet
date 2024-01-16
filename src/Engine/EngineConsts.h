#pragma once

#include "utils/Vector.h"

namespace engineDefaults {
    extern const Vector2I windowSize;
    extern const int objectsRadius;
    extern const int twoObjectsRadius;
    extern const int twoObjectsRadiusSquared;
    extern const Vector2I worldSize;
    extern const int maxObjectsNum;
    extern const Vector2F gravity;
    extern const int physicsSubSteps;
    extern const float wallsDamping;
    extern const float collisionRestitution;
    extern const float maxVelocity;
    extern const float linearDamping;
    extern const short numThreads;
    extern const float physicsInterval;
    extern Vector2F cameraDefaultPosition;
    extern float cameraMaxWorldViewSize;
}
