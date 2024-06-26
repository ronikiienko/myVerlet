#include "utils/Vector.h"

namespace engineDefaults {
    extern const Vector2I windowSize = Vector2I::cart(800, 800);

    extern const float rotationCircleRadius = 0.25;

    extern const float objectsRadius = 1;
    extern const float stickThickness = 0.5;
    extern const float objectsRadiusSquared = objectsRadius * objectsRadius;
    extern const float twoObjectsRadius = objectsRadius + objectsRadius;
    extern const float twoObjectsRadiusSquared = twoObjectsRadius * twoObjectsRadius;

    extern const Vector2I worldSize = Vector2I::cart(1200, 900);
    extern const int maxObjectsNum = 200000;
    extern const Vector2F gravity = Vector2F::cart(0, 0);
    extern const int physicsSubSteps = 8;
    extern const float wallsDamping = 0.8f;
    extern const float collisionRestitution = 0.45f;
    extern const float maxVelocity = 4;
    extern const float linearDamping = 0.9992f;
    extern const short numThreads = 4;
    // actually, only thing it does is affecting how much given m_acceleration affects velocity
    // To make it affect more, increase this value
    extern const float physicsInterval = 1.0f;

    extern const Vector2F cameraDefaultPosition = Vector2F::cart(0, 0);
    extern const float cameraMaxWorldViewSize = 400;
}
