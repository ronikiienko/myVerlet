#pragma once

#include "AtomWorld.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "utils/Grid.h"
#include "PerformanceMonitor.h"

class Physics {
private:
    AtomWorld &atomWorld;
    IdGrid grid;
    ThreadPool &threadPool;
    PerformanceMonitor &performanceMonitor;

    void updatePositionsConstraint(float dt) {
        const Rectangle bounds = atomWorld.getBoundsF();
        const int objectsCount = atomWorld.getObjectsCount();
        threadPool.dispatch(objectsCount, [this, &bounds, dt](int start, int end) {
            const float minX = bounds.getX1() + consts::objectsRadius;
            const float maxX = bounds.getX2() - consts::objectsRadius;
            const float minY = bounds.getY1() + consts::objectsRadius;
            const float maxY = bounds.getY2() - consts::objectsRadius;
            atomWorld.forEachObject([&bounds, dt, minX, maxX, minY, maxY](BaseObject& object, int i) {
                // TODO when all grid filled with objects, you can see that some start falling faster and some slower (on lower gravity levels like 10) - this is because of floats precision
                if (!object.isPinned) {
                    object.accelerate(consts::gravity);
                    object.update(dt);
                }

                // problem was that for example: atomWorld is 100x100. Then both objects are outside of field on same direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
                // offset is trying to fix this problem
                const float offset = static_cast<float>(i) * 1e-6f;

                const Vector2 newVelocity = object.getVelocity() * consts::wallsDamping;
                if (object.posCurr.x < minX) {
                    object.posCurr.x = bounds.getX1() + consts::objectsRadius + offset;
                    object.posOld.x = object.posCurr.x + newVelocity.x;
                } else if (object.posCurr.x > maxX) {
                    object.posCurr.x = bounds.getX2() - consts::objectsRadius - offset;
                    object.posOld.x = object.posCurr.x + newVelocity.x;
                }

                if (object.posCurr.y < minY) {
                    object.posCurr.y = bounds.getY1() + consts::objectsRadius + offset;
                    object.posOld.y = object.posCurr.y + newVelocity.y;
                } else if (object.posCurr.y > maxY) {
                    object.posCurr.y = bounds.getY2() - consts::objectsRadius - offset;
                    object.posOld.y = object.posCurr.y + newVelocity.y;
                }

            }, start, end);
        });
    }

//    void applyGravity() {
//        threadPool.dispatch(atomWorld.getObjectsCount(), [this](int start, int end) {
//            atomWorld.forEachObject([](BaseObject& object, int i) {
//                if (!object.isPinned) object.accelerate(gravity);
//            }, start, end);
//        });
//    }
//
//    void applyConstraints() {
//        const Rectangle bounds = atomWorld.getBoundsF();
//        const int objectsCount = atomWorld.getObjectsCount();
//        threadPool.dispatch(objectsCount, [this, &bounds](int start, int end) {
//            atomWorld.forEachObject([&bounds](BaseObject& object, int i) {
//                // problem was that for example: atomWorld is 100x100. Then both objects are outside of field on same direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
//                // offset is trying to fix this problem
//                const float offset = static_cast<float>(i) * 1e-6f;
//
//                const Vector2 velocity = (object.posCurr - object.posOld) * wallsDamping;
//                if (object.posCurr.x < bounds.getX1() + objectsRadius) {
//                    object.posCurr.x = bounds.getX1() + objectsRadius + offset;
//                    object.posOld.x = object.posCurr.x + velocity.x;
//                } else if (object.posCurr.x > bounds.getX2() - objectsRadius) {
//                    object.posCurr.x = bounds.getX2() - objectsRadius - offset;
//                    object.posOld.x = object.posCurr.x + velocity.x;
//                }
//
//                if (object.posCurr.y < bounds.getY1() + objectsRadius) {
//                    object.posCurr.y = bounds.getY1() + objectsRadius + offset;
//                    object.posOld.y = object.posCurr.y + velocity.y;
//                } else if (object.posCurr.y > bounds.getY2() - objectsRadius) {
//                    object.posCurr.y = bounds.getY2() - objectsRadius - offset;
//                    object.posOld.y = object.posCurr.y + velocity.y;
//                }
//            }, start, end);
//        });
//    }
//    void updatePositions(float dt) {
//        threadPool.dispatch(atomWorld.getObjectsCount(), [this, dt](int start, int end) {
//            atomWorld.forEachObject([dt](BaseObject& object, int i) {
//                if (!object.isPinned) object.update(dt);
//            }, start, end);
//        });
//    }

    static void solveContact(BaseObject &obj1, BaseObject &obj2) {
        const Vector2 vectorBetween = obj1.posCurr - obj2.posCurr;
        const float dist2 = vectorBetween.magnitude2();
        // Check overlapping
        if (dist2 < consts::twoObjectsRadiusSquared) {
            obj1.onCollision();
            obj2.onCollision();
            const float dist = std::sqrt(dist2);
            if (dist == 0) return;
            const Vector2 normal = vectorBetween / dist;
            const float delta = 0.5f * consts::collisionRestitution * (dist - consts::twoObjectsRadius);
            // Update positions
            if (!obj1.isPinned) obj1.posCurr -= normal * delta;
            if (!obj2.isPinned) obj2.posCurr += normal * delta;
        }
    }
//    void solveContact(BaseObject &obj1, BaseObject &obj2) {
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
//                solveContact(atomWorld.getObject(id1), atomWorld.getObject(id2));
//            });
//        });
//    }
    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2) {
        for (int i = 0; i < cell1.activeCount; i++) {
            const int id1 = cell1.ids[i];
            for (int j = 0; j < cell2.activeCount; j++) {
                const int id2 = cell2.ids[j];
                if (id1 == id2) continue;
                solveContact(atomWorld.getObject(id1), atomWorld.getObject(id2));
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





//    void rebuildGrid() {
//        grid.clear();
//        threadPool.dispatch(atomWorld.getObjectsCount(), [this](int start, int end){
//            atomWorld.forEachObject([this](BaseObject& object, int i) {
//                grid.insert(i, object.posCurr.x, object.posCurr.y);
//            }, start, end);
//        });
//    }
    void rebuildGrid() {
        performanceMonitor.start("grid clear");
        threadPool.dispatch(grid.length, [this](int start, int end){
            grid.clear(start, end);
        });
        performanceMonitor.end("grid clear");
        performanceMonitor.start("grid build");
        atomWorld.forEachObject([this](BaseObject& object, int i) {
            grid.insert(i, object.posCurr.x, object.posCurr.y);
        });
        performanceMonitor.end("grid build");

    }

    void constraintSticks() {
//        atomWorld.forEachStick([](BaseStick& stick, int i) {
//            stick.constraint();
//        });
        threadPool.dispatch(atomWorld.getSticksCount(), [this](int start, int end) {
            atomWorld.forEachStick([](BaseStick& stick, int i) {
                stick.constraint();
            }, start, end);
        });
    }

public:
    explicit Physics(AtomWorld &atomWorld, ThreadPool &threadPool, PerformanceMonitor& performanceMonitor)
            : atomWorld(atomWorld), grid(consts::collisionGridWidth, consts::collisionGridHeight, atomWorld.getBoundsI()), threadPool(threadPool), performanceMonitor(performanceMonitor) {}

    void update() {
        const float subStepDt = consts::physicsInterval / consts::physicsSubSteps;

        for (int i = 0; i < consts::physicsSubSteps; i++) {

            performanceMonitor.start("gravityConstraintsUpdate");
            updatePositionsConstraint(subStepDt);
            performanceMonitor.end("gravityConstraintsUpdate");

            performanceMonitor.start("sticks");
            constraintSticks();
            performanceMonitor.end("sticks");

            performanceMonitor.start("grid");
            rebuildGrid();
            performanceMonitor.end("grid");

            performanceMonitor.start("collisions");
            solveCollisions();
            performanceMonitor.end("collisions");

        }
    }
};