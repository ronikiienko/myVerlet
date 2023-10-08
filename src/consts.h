#pragma once

#include "modules/Vector.h"

constexpr Vector2 gravity = Vector2::fromCartesian(0, 1000);

const int windowWidth = 1000;
const int windowHeight = 1000;

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.75f;
const float collisionsDamping = 0.8f;