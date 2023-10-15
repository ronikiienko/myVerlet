#pragma once

#include "modules/Vector.h"
#include "modules/Rectangle.h"

const int maxObjectNum = 140000;
const int maxSticksNum = 100000;

constexpr Vector2 gravity = Vector2::fromCartesian(0, 200);

constexpr RectangleI worldBounds = RectangleI::fromSize(0,0,2500,1200);
constexpr RectangleI windowBounds = RectangleI::fromSize(0,0,2500,1200);

constexpr int minRadius = 2;
constexpr int maxRadius = 2;

// collision grid when finding collisions is split on many threads (by columns). To avoid race conditions i use two pass method. This means that grid in fact will be split on numberOfThreads * 2 tasks. So, if we have 10threads
// and grid of, say, size 39 will mean that each thread gets 1 task, and 19 remain for "cleanup" function, which is inefficient, because cleanup function is not really good
// best if grid is divided by threads * 2 without remainder
constexpr int collisionGridWidth = worldBounds.getWidth() / (maxRadius * 2);
constexpr int collisionGridHeight = worldBounds.getHeight() / (maxRadius * 2);

const float physicsInterval = 0.016f;
const int physicsSubSteps = 8;

const float wallsDamping = 0.2f;
const float collisionsDamping = 0.2f;

const int seed = 40;

const int numThreads = 10;

bool logFps = true;