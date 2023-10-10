#pragma once

#include "modules/Vector.h"
#include "modules/Rectangle.h"

constexpr Vector2 gravity = Vector2::fromCartesian(0, 2);

const RectangleI worldBounds = RectangleI::fromSize(0,0,1000,1000);
const RectangleI windowBounds = RectangleI::fromSize(0,0,1000,1000);

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.75f;
const float collisionsDamping = 0.8f;