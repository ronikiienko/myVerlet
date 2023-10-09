#pragma once

#include "../World/World.h"
#include "SFML/Graphics/RenderWindow.hpp"

class Physics {
private:
    World &world;

    void applyConstraints() {
        const Rectangle bounds = world.getBounds();
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            const Vector2 velocity = (object.posCurr - object.posOld) * wallsDamping;
            if (object.posCurr.x < bounds.getX1() + object.radius) {
                object.posCurr.x = bounds.getX1() + object.radius;
                object.posOld.x = object.posCurr.x + velocity.x;
            }
            if (object.posCurr.x > bounds.getX2() - object.radius) {
                object.posCurr.x = bounds.getX2() - object.radius;
                object.posOld.x = object.posCurr.x + velocity.x;
            }
            if (object.posCurr.y < bounds.getY1() + object.radius) {
                object.posCurr.y = bounds.getY1() + object.radius;
                object.posOld.y = object.posCurr.y + velocity.y;
            }
            if (object.posCurr.y > bounds.getY2() - object.radius) {
                object.posCurr.y = bounds.getY2() - object.radius;
                object.posOld.y = object.posCurr.y + velocity.y;
            }
        }
    }

    void solveCollisions() {
        std::vector<VerletObject> &objects = world.getObjects();
        const int objectsCount = world.getObjectsCount();
        // Iterate on all objects
        for (int i{0}; i < objectsCount; i++) {
            VerletObject &obj1 = objects[i];
            // Iterate on object involved in new collision pairs
            for (int j{i + 1}; j < objectsCount; j++) {
                VerletObject &obj2 = objects[j];
                const Vector2 vectorBetween = obj1.posCurr - obj2.posCurr;
                const float dist2 = vectorBetween.magnitude2();
                const float min_dist = obj1.radius + obj2.radius;
                // Check overlapping
                if (dist2 < min_dist * min_dist) {
                    const float dist = sqrt(dist2);
                    const Vector2 normal = vectorBetween / dist;
                    const float massRatio1 = obj1.radius / (obj1.radius + obj2.radius);
                    const float massRatio2 = obj2.radius / (obj1.radius + obj2.radius);
                    const float delta = 0.5f * collisionsDamping * (dist - min_dist);
                    // Update positions
                    obj1.posCurr -= normal * (massRatio2 * delta);
                    obj2.posCurr += normal * (massRatio1 * delta);
                }
            }
        }
    }


    void applyGravity() {
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            object.accelerate(gravity);
        }
    }

    void updatePositions(float dt) {
        std::vector<VerletObject> &objects = world.getObjects();
        for (VerletObject &object: objects) {
            object.update(dt);
        }
    }

public:
    explicit Physics(World &world) : world(world) {};

    void update() {
        const float subStepDt = physicsInterval / physicsSubSteps;
        for (int i = 0; i < physicsSubSteps; i++) {
            applyGravity();
            solveCollisions();
            applyConstraints();
            updatePositions(subStepDt);
        }
    }
};