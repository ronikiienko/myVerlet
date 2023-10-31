#pragma once

#include "../World/World.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "../modules/Grid.h"
#include "../modules/ThreadPool.h"
#include "../PerformanceMonitor/PerformanceMonitor.h"

class Physics {
private:
    World &world;
    IdGrid grid;
    ThreadPool &threadPool;
    PerformanceMonitor &performanceMonitor;

    void applyConstraints() {
        const Rectangle bounds = world.getBoundsF();
        const int objectsCount = world.getObjectsCount();
        threadPool.dispatch(objectsCount, [this, &bounds](int start, int end) {
            world.forEachObject([&bounds](VerletObject& object, int i) {
                // problem was that for example: world is 100x100. Then both objects are outside of field on same direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
                // offset is trying to fix this problem
                const float offset = static_cast<float>(i) * 1e-6f;

                const Vector2 velocity = (object.posCurr - object.posOld) * wallsDamping;
                if (object.posCurr.x < bounds.getX1() + object.radius) {
                    object.posCurr.x = bounds.getX1() + object.radius + offset;
                    object.posOld.x = object.posCurr.x + velocity.x;
                } else if (object.posCurr.x > bounds.getX2() - object.radius) {
                    object.posCurr.x = bounds.getX2() - object.radius - offset;
                    object.posOld.x = object.posCurr.x + velocity.x;
                }

                if (object.posCurr.y < bounds.getY1() + object.radius) {
                    object.posCurr.y = bounds.getY1() + object.radius + offset;
                    object.posOld.y = object.posCurr.y + velocity.y;
                } else if (object.posCurr.y > bounds.getY2() - object.radius) {
                    object.posCurr.y = bounds.getY2() - object.radius - offset;
                    object.posOld.y = object.posCurr.y + velocity.y;
                }
            }, start, end);
        });
    }

    void solveContact(VerletObject &obj1, VerletObject &obj2) {
        const Vector2 vectorBetween = obj1.posCurr - obj2.posCurr;
        const float dist2 = vectorBetween.magnitude2();
        const float min_dist = obj1.radius + obj2.radius;
        // Check overlapping
        if (dist2 < min_dist * min_dist) {
            const float dist = std::sqrt(dist2);
            if (dist == 0) return;
            const Vector2 normal = vectorBetween / dist;
            const float delta = 0.5f * collisionRestitution * (dist - min_dist);
            // Update positions
            if (!obj1.isPinned) obj1.posCurr -= normal * delta;
            if (!obj2.isPinned) obj2.posCurr += normal * delta;
        }
    }
//    void solveContact(VerletObject &obj1, VerletObject &obj2) {
//        Vector2 move = obj1.posCurr - obj2.posCurr;
//        const float dist2 = move.magnitude2();
//        const float min_dist = obj1.radius + obj2.radius;
//        // Check overlapping
//        if (dist2 < min_dist * min_dist) {
//            const float dist = sqrt(dist2);
//            move *= 0.5f * collisionRestitution * (dist - min_dist) / dist;
//            // Update positions
//            if (!obj1.isPinned) obj1.posCurr -= move;
//            if (!obj2.isPinned) obj2.posCurr += move;
//        }
//    }

//    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2) {
//        cell1.forEachId([&](int id1, int i){
//            cell2.forEachId([&](int id2, int i){
//                if (id1 == id2) return;
//                solveContact(world.getObject(id1), world.getObject(id2));
//            });
//        });
//    }
    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2) {
        for (int i = 0; i < cell1.activeCount; i++) {
            const int id1 = cell1.ids[i];
            for (int j = 0; j < cell2.activeCount; j++) {
                const int id2 = cell2.ids[j];
                if (id1 == id2) continue;
                solveContact(world.getObject(id1), world.getObject(id2));
            }
        }
    }

    void solveCollisionsSubgrid(int startX, int endX, int startY, int endY) {
        for (int i = startX; i < endX; i++) {
            for (int j = startY; j < endY; j++) {
                const Cell &cell1 = grid.get(i, j);
                solveCollisionsTwoCells(cell1, cell1);
                if (i + 1 < grid.width && j - 1 >= 0) {
                    solveCollisionsTwoCells(cell1, grid.get(i + 1, j - 1));
                }
                if (i + 1 < grid.width) {
                    solveCollisionsTwoCells(cell1, grid.get(i + 1, j));
                }
                if (i + 1 < grid.width && j + 1 < grid.height) {
                    solveCollisionsTwoCells(cell1, grid.get(i + 1, j + 1));
                }
                if (j + 1 < grid.height) {
                    solveCollisionsTwoCells(cell1, grid.get(i, j + 1));
                }
            }
        }
    }

    void solveCollisions() {
        const int threadCount = threadPool.threadsNum;
        const int sliceCount = threadCount * 2;
        const int sliceSize = grid.width / sliceCount;
        //  to avoid data races - process in two passes. So that no threads are assigned to neighbouring columns
        for (int i = 0; i < threadCount; i++) {
            threadPool.addTask([i, sliceSize, this](){
                int startCol = (i * 2) * sliceSize;
                int endCol = startCol + sliceSize;
                solveCollisionsSubgrid(startCol, endCol, 0, grid.height);
            });
        }
        threadPool.waitForCompletion();

        for (int i = 0; i < threadCount; i++) {
            threadPool.addTask([i, sliceSize, this]() {
                int startCol = (i * 2 + 1) * sliceSize;
                int endCol = startCol + sliceSize;
                solveCollisionsSubgrid(startCol, endCol, 0, grid.height);
            });
        };
        threadPool.waitForCompletion();

        if (sliceSize * sliceCount < grid.width) {
            threadPool.addTask([sliceSize, sliceCount, this](){
                int startCol = sliceSize * sliceCount;
                int endCol = grid.width;
                solveCollisionsSubgrid(startCol, endCol, 0, grid.height);
            });
        }
        threadPool.waitForCompletion();
    }

    // TODO when all grid filled with objects, you can see that some start falling faster and some slower (on lower gravity levels like 10)
    void applyGravity() {
        threadPool.dispatch(world.getObjectsCount(), [this](int start, int end) {
            world.forEachObject([](VerletObject& object, int i) {
                if (!object.isPinned) object.accelerate(gravity);
            }, start, end);
        });
    }

    void updatePositions(float dt) {
        threadPool.dispatch(world.getObjectsCount(), [this, dt](int start, int end) {
            world.forEachObject([dt](VerletObject& object, int i) {
                if (!object.isPinned) object.update(dt);
            }, start, end);
        });
    }

//    void rebuildGrid() {
//        grid.clear();
//        threadPool.dispatch(world.getObjectsCount(), [this](int start, int end){
//            world.forEachObject([this](VerletObject& object, int i) {
//                grid.insert(i, object.posCurr.x, object.posCurr.y);
//            }, start, end);
//        });
//    }
    void rebuildGrid() {
//        performanceMonitor.start("grid clear");
        grid.clear();
//        performanceMonitor.end("grid clear");

//        performanceMonitor.start("grid build");
        world.forEachObject([this](VerletObject& object, int i) {
            grid.insert(i, object.posCurr.x, object.posCurr.y);
        });
//        performanceMonitor.end("grid build");

    }

    void constraintSticks() {
//        world.forEachStick([](VerletStick& stick, int i) {
//            stick.constraint();
//        });
        threadPool.dispatch(world.getSticksCount(), [this](int start, int end) {
            world.forEachStick([](VerletStick& stick, int i) {
                stick.constraint();
            }, start, end);
        });
    }

public:
    explicit Physics(World &world, ThreadPool &threadPool, PerformanceMonitor& performanceMonitor)
            : world(world), grid(collisionGridWidth, collisionGridHeight, world.getBoundsI()), threadPool(threadPool), performanceMonitor(performanceMonitor) {}

    void update() {
        const float subStepDt = physicsInterval / physicsSubSteps;

        for (int i = 0; i < physicsSubSteps; i++) {
            applyGravity();
            performanceMonitor.start("sticks");
            constraintSticks();
            performanceMonitor.end("sticks");

            applyConstraints();

            performanceMonitor.start("grid");
            rebuildGrid();
            performanceMonitor.end("grid");

            performanceMonitor.start("collisions");
            solveCollisions();
            performanceMonitor.end("collisions");

            updatePositions(subStepDt);
        }
    }
};