#pragma once

#include "modules/Vector.h"
#include "modules/Rectangle.h"

const int maxObjectNum = 5000;
const int maxSticksNum = 100;

constexpr Vector2 gravity = Vector2::fromCartesian(0, 50);

const RectangleI worldBounds = RectangleI::fromSize(0,0,2500,1200);
const RectangleI windowBounds = RectangleI::fromSize(0,0,2500,1200);

const int minRadius = 4;
const int maxRadius = 4;

const int collisionGridWidth = worldBounds.getWidth() / (maxRadius * 2);
const int collisionGridHeight = worldBounds.getHeight() / (maxRadius * 2);

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.2f;
const float collisionsDamping = 0.2f;

const int seed = 401;

const int numThreads = 10;