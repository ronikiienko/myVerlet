#pragma once

#include "utils/Vector.h"

namespace engineDefaults {
    extern const Vector2I windowSize;
    extern const float rotationCircleRadius;
    extern const float objectsRadius;
    extern const float objectsRadiusSquared;
    extern const float twoObjectsRadius;
    extern const float twoObjectsRadiusSquared;
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
