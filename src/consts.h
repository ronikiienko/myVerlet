#pragma once

#include "modules/Vector.h"

constexpr Vector2 gravity = Vector2::fromCartesian(0, 10);

const Vector2 windowDims = Vector2::fromCartesian(1000, 1000);
const Vector2 worldDims = Vector2::fromCartesian(1000, 1000);

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.75f;
const float collisionsDamping = 0.8f;