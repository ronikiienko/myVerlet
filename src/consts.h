#pragma once

#include "modules/Vector.h"
#include "modules/Rectangle.h"

const int maxObjectNum = 40000;
const int maxSticksNum = 100;

constexpr Vector2 gravity = Vector2::fromCartesian(0, 100);

const RectangleI worldBounds = RectangleI::fromSize(0,0,1000,1000);
const RectangleI windowBounds = RectangleI::fromSize(0,0,1000,1000);

const int gridX = 250;
const int gridY = 250;

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.8f;
const float collisionsDamping = 0.8f;