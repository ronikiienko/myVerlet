#pragma once

#include <thread>
#include "../World/World.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "../modules/Grid.h"
#include "../modules/thread_pool.hpp"

class Physics {
private:
    World &world;
    IdGrid grid;
    tp::ThreadPool &threadPool;

    void applyConstraints() {
        const Rectangle bounds = world.getBoundsF();
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            const Vector2 velocity = (object.posCurr - object.posOld) * wallsDamping;
            if (object.posCurr.x < bounds.getX1() + object.radius) {
                object.posCurr.x = bounds.getX1() + object.radius;
                object.posOld.x = object.posCurr.x + velocity.x;
            } else if (object.posCurr.x > bounds.getX2() - object.radius) {
                object.posCurr.x = bounds.getX2() - object.radius;
                object.posOld.x = object.posCurr.x + velocity.x;
            }

            if (object.posCurr.y < bounds.getY1() + object.radius) {
                object.posCurr.y = bounds.getY1() + object.radius;
                object.posOld.y = object.posCurr.y + velocity.y;
            } else if (object.posCurr.y > bounds.getY2() - object.radius) {
                object.posCurr.y = bounds.getY2() - object.radius;
                object.posOld.y = object.posCurr.y + velocity.y;
            }
        }
    }

    void solveContact(VerletObject &obj1, VerletObject &obj2) {
        const Vector2 vectorBetween = obj1.posCurr - obj2.posCurr;
        const float dist2 = vectorBetween.magnitude2();
        const float min_dist = obj1.radius + obj2.radius;
        // Check overlapping
        if (dist2 < min_dist * min_dist) {
            const float dist = sqrt(dist2);
            const Vector2 normal = vectorBetween / dist;
            const float delta = 0.5f * collisionsDamping * (dist - min_dist);
            // Update positions
            if (!obj1.isPinned) obj1.posCurr -= normal * delta;
            if (!obj2.isPinned) obj2.posCurr += normal * delta;
        }
    }

    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2, std::vector<VerletObject> &objects) {
        for (int i = 0; i < cell1.activeCount; ++i) {
            int index1 = cell1.ids[i];
            for (int j = 0; j < cell2.activeCount; ++j) {
                int index2 = cell2.ids[j];
                if (index1 == index2) continue;

                solveContact(objects[index1], objects[index2]);
            }
        }
    }

    void solveCollisionsSubgrid(int startX, int endX, int startY, int endY) {
        std::vector<VerletObject> &objects = world.getObjects();
        for (int i = startX; i < endX; i++) {
            for (int j = startY; j < endY; j++) {
                const Cell &cell1 = grid.get(i, j);
                solveCollisionsTwoCells(cell1, cell1, objects);
                if (i + 1 < grid.width && j - 1 >= 0) {
                    solveCollisionsTwoCells(cell1, grid.get(i + 1, j - 1), objects);
                }
                if (i + 1 < grid.width) {
                    solveCollisionsTwoCells(cell1, grid.get(i + 1, j), objects);
                }
                if (i + 1 < grid.width && j + 1 < grid.height) {
                    solveCollisionsTwoCells(cell1, grid.get(i + 1, j + 1), objects);
                }
                if (j + 1 < grid.height) {
                    solveCollisionsTwoCells(cell1, grid.get(i, j + 1), objects);
                }
            }
        }
    }

    void solveCollisions() {

//        sf::Clock clock;
        rebuildGrid();
//        const long long elapsed = clock.restart().asMicroseconds();
//        std::cout << "elapsed: " << elapsed * 8 << '\n';
        int segment = grid.width / static_cast<int>(numThreads);

        for (int i = 0; i < numThreads; i++) {
            int startX = segment * i;
            int endX = i + 1 == numThreads ? grid.width : segment * (i + 1);

            threadPool.addTask([startX, endX, this]() {
                solveCollisionsSubgrid(startX, endX, 0, grid.height);
            });
        }


        threadPool.waitForCompletion();

    }


    // TODO when all grid filled with objects, you can see that some start falling faster and some slower (on lower gravity levels like 10)
    void applyGravity() {
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            if (!object.isPinned) object.accelerate(gravity);
        }
    }

    void updatePositions(float dt) {
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            if (!object.isPinned) object.update(dt);
        }
    }

    void rebuildGrid() {
        grid.clear();
        std::vector<VerletObject> &objects = world.getObjects();
        const int objectsCount = world.getObjectsCount();
        for (int i = 0; i < objectsCount; i++) {
            grid.insert(i, objects[i].posCurr.x, objects[i].posCurr.y);
        }
    }

    void constraintSticks() {
        std::vector<VerletStick> &sticks = world.getSticks();
        for (VerletStick &stick: sticks) {
            stick.constraint();
        }
    }

public:
    explicit Physics(World &world, tp::ThreadPool& threadPool)
            : world(world), grid(collisionGridWidth, collisionGridHeight, world.getBoundsI()), threadPool(threadPool) {}

    void update() {
        const float subStepDt = physicsInterval / physicsSubSteps;

        for (int i = 0; i < physicsSubSteps; i++) {
            applyGravity();
            applyConstraints();
            constraintSticks();
            solveCollisions();
            updatePositions(subStepDt);
        }
    }
};