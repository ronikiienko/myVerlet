#pragma once

#include "modules/Vector.h"
#include "modules/Rectangle.h"

const int maxObjectNum = 140000;
const int maxSticksNum = 100000;

constexpr Vector2 gravity = Vector2::fromCartesian(0, 100);

constexpr RectangleI worldBounds = RectangleI::fromSize(0,0,2500,1200);
constexpr RectangleI windowBounds = RectangleI::fromSize(0,0,2500,1200);

constexpr int minRadius = 2;
constexpr int maxRadius = 2;

constexpr int collisionGridWidth = worldBounds.getWidth() / (maxRadius * 2);
constexpr int collisionGridHeight = worldBounds.getHeight() / (maxRadius * 2);

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.2f;
const float collisionsDamping = 0.2f;

const int seed = 401;

const int numThreads = 10;

bool logFps = true;