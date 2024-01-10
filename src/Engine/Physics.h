#pragma once

#include "Scene.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "utils/Grid.h"
#include "PerformanceMonitor.h"

class Physics {
private:
    Scene &scene;
    IdGrid& grid;
    ThreadPool &threadPool;
    PerformanceMonitor &performanceMonitor;

    int subSteps = engineDefaults::physicsSubSteps;
    bool collisionsEnabled = true;
    float maxVelocity = engineDefaults::maxVelocity;
    Vector2F gravity = engineDefaults::gravity;
    float collisionRestitution = engineDefaults::collisionRestitution;
    float linearDamping = engineDefaults::linearDamping;
    float wallsDamping = engineDefaults::wallsDamping;

    void updatePositionsConstraint(float dt) {
        const Vector2F size = scene.getSizeF();
        const int objectsCount = scene.getObjectsCount();
        threadPool.dispatch(objectsCount, [this, &size, dt](int start, int end) {
            const float minX = 0 + engineDefaults::objectsRadius;
            const float maxX = size.x - engineDefaults::objectsRadius;
            const float minY = 0 + engineDefaults::objectsRadius;
            const float maxY = size.y - engineDefaults::objectsRadius;
            scene.forEachBasicDetails([&size, dt, minX, maxX, minY, maxY, this](BasicDetails &object, int i) {
                // TODO when all grid filled with objects, you can see that some start falling faster and some slower (on lower gravity levels like 10) - this is because of floats precision
                if (!object.isPinned) {
                    object.accelerate(gravity);
//                    object.update(dt);
                    Vector2F velocity = object.posCurr - object.posOld;
                    velocity *= linearDamping;
                    // TODO review maby limiting not needed
                    velocity.limitMagnitude(maxVelocity);

                    object.posOld = object.posCurr;
                    object.posCurr += velocity + (object.acceleration * (dt * dt));

                    object.acceleration = Vector2F::cart();
                }

                // problem was that for example: scene is 100x100. Then both objects are outside of field on same direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
                // offset is trying to fix this problem
                const float offset = static_cast<float>(i) * 1e-6f;

                const Vector2F newVelocity = object.getVelocity() * wallsDamping;
                if (object.posCurr.x < minX) {
                    object.posCurr.x = 0 + engineDefaults::objectsRadius + offset;
                    object.posOld.x = object.posCurr.x + newVelocity.x;
                } else if (object.posCurr.x > maxX) {
                    object.posCurr.x = size.x - engineDefaults::objectsRadius - offset;
                    object.posOld.x = object.posCurr.x + newVelocity.x;
                }

                if (object.posCurr.y < minY) {
                    object.posCurr.y = 0 + engineDefaults::objectsRadius + offset;
                    object.posOld.y = object.posCurr.y + newVelocity.y;
                } else if (object.posCurr.y > maxY) {
                    object.posCurr.y = size.y - engineDefaults::objectsRadius - offset;
                    object.posOld.y = object.posCurr.y + newVelocity.y;
                }

            }, start, end);
        });
    }

//    void applyGravity() {
//        threadPool.dispatch(scene.getObjectsCount(), [this](int start, int end) {
//            scene.forEachObject([](BaseObject& object, int i) {
//                if (!object.isPinned) object.accelerate(gravity);
//            }, start, end);
//        });
//    }
//
//    void applyConstraints() {
//        const Rectangle bounds = scene.getSizeF();
//        const int objectsCount = scene.getObjectsCount();
//        threadPool.dispatch(objectsCount, [this, &bounds](int start, int end) {
//            scene.forEachObject([&bounds](BaseObject& object, int i) {
//                // problem was that for example: scene is 100x100. Then both objects are outside of field on same direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
//                // offset is trying to fix this problem
//                const float offset = static_cast<float>(i) * 1e-6f;
//
//                const Vector2F velocity = (object.posCurr - object.posOld) * wallsDamping;
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
//        threadPool.dispatch(scene.getObjectsCount(), [this, dt](int start, int end) {
//            scene.forEachObject([dt](BaseObject& object, int i) {
//                if (!object.isPinned) object.update(dt);
//            }, start, end);
//        });
//    }

    void solveContact(BasicDetails &obj1, BasicDetails &obj2) {
        const Vector2F vectorBetween = obj1.posCurr - obj2.posCurr;
        const float dist2 = vectorBetween.magnitude2();
        // Check overlapping
        if (dist2 < engineDefaults::twoObjectsRadiusSquared) {
            obj1.parent->onCollision(obj2.parent);
            obj2.parent->onCollision(obj1.parent);

            const float dist = std::sqrt(dist2);
            if (dist == 0) return;
            const Vector2F normal = vectorBetween / dist;
            const float delta = 0.5f * collisionRestitution * (dist - engineDefaults::twoObjectsRadius);
            // Update positions
            if (!obj1.isPinned) obj1.posCurr -= normal * delta;
            if (!obj2.isPinned) obj2.posCurr += normal * delta;
        }
    }
//    void solveContact(BaseObject &obj1, BaseObject &obj2) {
//        Vector2F move = obj1.posCurr - obj2.posCurr;
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
//                solveContact(scene.getObject(id1), scene.getObject(id2));
//            });
//        });
//    }
    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2) {
        for (int i = 0; i < cell1.activeCount; i++) {
            const int id1 = cell1.ids[i];
            for (int j = 0; j < cell2.activeCount; j++) {
                const int id2 = cell2.ids[j];
                if (id1 == id2) continue;
                solveContact(scene.getBasicDetails(id1), scene.getBasicDetails(id2));
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
            threadPool.addTask([i, sliceSize, this]() {
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
            threadPool.addTask([sliceSize, sliceCount, this]() {
                int startCol = sliceSize * sliceCount;
                int endCol = grid.width;
                solveCollisionsSubgrid(startCol, endCol, 0, grid.height);
            });
        }
        threadPool.waitForCompletion();
    }


//    void rebuildGrid() {
//        grid.clear();
//        threadPool.dispatch(scene.getObjectsCount(), [this](int start, int end){
//            scene.forEachObject([this](BaseObject& object, int i) {
//                grid.insert(i, object.posCurr.x, object.posCurr.y);
//            }, start, end);
//        });
//    }


public:
    explicit Physics(Scene &scene, ThreadPool &threadPool, PerformanceMonitor &performanceMonitor)
            : scene(scene),
              grid(scene.grid),
              threadPool(threadPool), performanceMonitor(performanceMonitor) {}

    void update() {
        int localSubSteps = subSteps;
        bool localCollisionsEnabled = collisionsEnabled;

        const float subStepDt = engineDefaults::physicsInterval / static_cast<float>(localSubSteps);

        for (int i = 0; i < localSubSteps; i++) {
            performanceMonitor.start("gravityConstraintsUpdate");
            updatePositionsConstraint(subStepDt);
            performanceMonitor.end("gravityConstraintsUpdate");

            performanceMonitor.start("grid");
            scene.rebuildGrid();
            performanceMonitor.end("grid");

            if (localCollisionsEnabled) {
                performanceMonitor.start("collisions");
                solveCollisions();
                performanceMonitor.end("collisions");
            }
        }
    }

    // warning: if setting substeps from onCollision (for example), then subSteps number can change while update is running.
    // For that i use localSubSteps variable
    void setSubSteps(int value) {
        if (value > 0 && value < 16) {
            subSteps = value;
        } else {
            throw std::runtime_error("Substeps number should be between 1 and 16");
        }
    }

    [[nodiscard]] int getSubSteps() const {
        return subSteps;
    }

    void setCollisionsEnabled(bool enabled) {
        collisionsEnabled = enabled;
    }

    [[nodiscard]] bool getCollisionsEnabled() const {
        return collisionsEnabled;
    }

    // limit velocity of each object on each update() call.
    // This can prevent full chaos.
    void setMaxVelocity(float value) {
        maxVelocity = value;
    }

    [[nodiscard]] float getMaxVelocity() const {
        return maxVelocity;
    }

    // set gravity.
    // high values can cause objects to pass through each other and other weird stuff
    void setGravity(Vector2F value) {
        gravity = value;
    }

    [[nodiscard]] Vector2F getGravity() const {
        return gravity;
    }

    // adjusts how much objects will be "splitted" when resolving collisions.
    // 0 - not splitted no collision resolving happens. 1 - objects are fully splitted
    void setCollisionRestitution(float value) {
        if (value < 0 || value > 1) {
            throw std::runtime_error("Collision restitution should be between 0 and 1");
        }
        collisionRestitution = value;
    }

    [[nodiscard]] float getCollisionRestitution() const {
        return collisionRestitution;
    }

    // We multiply velocity of each object by this value on each substep.
    // Can help a bit if simulation goes to chaos
    // Value from 0 to 1
    void setLinearDamping(float value) {
        if (value < 0 || value > 1) {
            throw std::runtime_error("Linear damping should be between 0 and 1");
        }
        linearDamping = value;
    }

    [[nodiscard]] float getLinearDamping() const {
        return linearDamping;
    }

    // We multiply velocity of object by this value when it hits walls
    // Value from 0 to 1
    void setWallsDamping(float value) {
        if (value < 0 || value > 1) {
            throw std::runtime_error("Walls damping should be between 0 and 1");
        }
        wallsDamping = value;
    }

    [[nodiscard]] float getWallsDamping() const {
        return wallsDamping;
    }
};

// grid(),