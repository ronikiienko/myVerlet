#include "utils/Vector.h"

namespace engineDefaults {
    extern const Vector2I windowSize = Vector2I::cart(2400, 1000);
    extern const float objectsRadius = 2;
    extern const float twoObjectsRadius = objectsRadius + objectsRadius;
    extern const float twoObjectsRadiusSquared = twoObjectsRadius * twoObjectsRadius;

    extern const Vector2I worldSize = Vector2I::cart(2400, 1800);
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
    extern const float physicsInterval = 0.001f;

    extern const Vector2F cameraDefaultPosition = Vector2F::cart(0, 0);
    extern const float cameraMaxWorldViewSize = 400;
}
