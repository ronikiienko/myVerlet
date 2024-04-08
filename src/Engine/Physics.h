#pragma once

#include "Scene/Scene.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "utils/Grid.h"
#include "PerformanceMonitor.h"
#include "EngineConsts.h"

class Physics {
private:
    Scene &m_scene;
    IdGrid &m_grid;
    ThreadPool &m_threadPool;
    PerformanceMonitor &m_performanceMonitor;

    int m_subSteps = engineDefaults::physicsSubSteps;
    float m_collisionRestitution = engineDefaults::collisionRestitution;
    float m_linearDamping = engineDefaults::linearDamping;
    float m_wallsDamping = engineDefaults::wallsDamping;

    int m_subStepsCallbackInterval = 1;
    int m_subStepsCallbackCounter = 0;

    void constrainSticks() {
        m_scene.getStickStorage().forEachBasicStickDetails([this](BasicStickDetails& stick, int i) {
            BasicDetails &obj1 = m_scene.getObjectStorage().getBasicDetails(stick.m_id1);
            BasicDetails &obj2 = m_scene.getObjectStorage().getBasicDetails(stick.m_id2);
            const Vector2 vectorBetween = obj1.m_posCurr - obj2.m_posCurr;
            const float distanceBetween = vectorBetween.magnitude();

            const float stretch = distanceBetween - stick.m_length;
            if (stretch > stick.m_maxStretch) {
                m_scene.getStickStorage().removeStick(i);
            }

            const float diff = distanceBetween - stick.m_length;
            const float moveRatio = (diff / distanceBetween) / 2;

            const Vector2 offset = vectorBetween * moveRatio;

            if (!obj1.m_isPinned) obj1.m_posCurr -= offset;
            if (!obj2.m_isPinned) obj2.m_posCurr += offset;
        });
    }

    void updatePositionsConstraint(float dt) {
        const Vector2F size = m_scene.getSizeF();
        const int objectsCount = m_scene.getObjectStorage().getObjectsCount();
        m_threadPool.dispatch(objectsCount, [this, &size, dt](int start, int end) {
            const float minX = 0 + engineDefaults::objectsRadius;
            const float maxX = size.m_x - engineDefaults::objectsRadius;
            const float minY = 0 + engineDefaults::objectsRadius;
            const float maxY = size.m_y - engineDefaults::objectsRadius;
            m_scene.getObjectStorage().forEachBasicDetails([&size, dt, minX, maxX, minY, maxY, this](BasicDetails &object, int i) {
                // TODO when all m_grid filled with m_objects, you can see that some start falling faster and some slower (on lower m_gravity levels like 10) - this is because of floats precision
                if (!object.m_isPinned) {
                    object.accelerate(m_scene.getGravity());
//                    object.update(dt);
                    Vector2F velocity = object.m_posCurr - object.m_posOld;
                    velocity *= m_linearDamping;
                    // TODO review maby limiting not needed
                    velocity.limitMagnitudeModify(m_scene.getMaxVelocity());

                    object.m_posOld = object.m_posCurr;
                    object.m_posCurr += velocity + (object.m_acceleration * dt);

                    object.m_acceleration = Vector2F::cart();
                }
                switch (m_scene.getBoundaryType()) {
                    case BOUNDARY_TYPE::SOLID: {
                        // problem was that for example: m_scene is 100x100. Then both m_objects are outside of field on same m_direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
                        // offset is trying to fix this problem
                        const float offset = static_cast<float>(i) * 1e-6f;

                        const Vector2F newVelocity = object.getVelocity() * m_wallsDamping;
                        if (object.m_posCurr.m_x < minX) {
                            object.m_posCurr.m_x = minX + offset;
                            object.m_posOld.m_x = object.m_posCurr.m_x + newVelocity.m_x;
                        } else if (object.m_posCurr.m_x > maxX) {
                            object.m_posCurr.m_x = maxX - offset;
                            object.m_posOld.m_x = object.m_posCurr.m_x + newVelocity.m_x;
                        }

                        if (object.m_posCurr.m_y < minY) {
                            object.m_posCurr.m_y = minY + offset;
                            object.m_posOld.m_y = object.m_posCurr.m_y + newVelocity.m_y;
                        } else if (object.m_posCurr.m_y > maxY) {
                            object.m_posCurr.m_y = maxY - offset;
                            object.m_posOld.m_y = object.m_posCurr.m_y + newVelocity.m_y;
                        }
                    }
                        break;
                    case BOUNDARY_TYPE::WRAP: {
                        const Vector2F oldVelocity = object.m_posCurr - object.m_posOld;

                        if (object.m_posCurr.m_x < 0 || object.m_posCurr.m_x >= size.m_x) {
                            object.m_posCurr.m_x = std::fmod(object.m_posCurr.m_x + size.m_x, size.m_x);
                            object.setVelocity(oldVelocity);
                        }
                        if (object.m_posCurr.m_y < 0 || object.m_posCurr.m_y >= size.m_y) {
                            object.m_posCurr.m_y = std::fmod(object.m_posCurr.m_y + size.m_y, size.m_y);
                            object.setVelocity(oldVelocity);
                        }
                    }
                        break;
                    case BOUNDARY_TYPE::REMOVE: {
                        // problem was that for example: m_scene is 100x100. Then both m_objects are outside of field on same m_direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
                        // offset is trying to fix this problem
                        const float offset = static_cast<float>(i) * 1e-6f;

                        const Vector2F newVelocity = object.getVelocity() * m_wallsDamping;
                        if (object.m_posCurr.m_x < minX) {
                            object.m_posCurr.m_x = minX + offset;
                            object.m_posOld.m_x = object.m_posCurr.m_x + newVelocity.m_x;
                            m_scene.getObjectStorage().removeObject(i);
                        } else if (object.m_posCurr.m_x > maxX) {
                            object.m_posCurr.m_x = maxX - offset;
                            object.m_posOld.m_x = object.m_posCurr.m_x + newVelocity.m_x;
                            m_scene.getObjectStorage().removeObject(i);
                        }

                        if (object.m_posCurr.m_y < minY) {
                            object.m_posCurr.m_y = minY + offset;
                            object.m_posOld.m_y = object.m_posCurr.m_y + newVelocity.m_y;
                            m_scene.getObjectStorage().removeObject(i);
                        } else if (object.m_posCurr.m_y > maxY) {
                            object.m_posCurr.m_y = maxY - offset;
                            object.m_posOld.m_y = object.m_posCurr.m_y + newVelocity.m_y;
                            m_scene.getObjectStorage().removeObject(i);
                        }
                    }
                        break;
                }
            }, start, end);
        });
    }

    template<bool WithCallback>
    void solveCollisions() {
        auto solveCont = [&](int id1, int id2) {
            BasicDetails &obj1 = m_scene.getObjectStorage().getBasicDetails(id1);
            BasicDetails &obj2 = m_scene.getObjectStorage().getBasicDetails(id2);
            const Vector2F vectorBetween = obj1.m_posCurr - obj2.m_posCurr;
            const float dist2 = vectorBetween.magnitude2();
            // Check overlapping
            if (dist2 < engineDefaults::twoObjectsRadiusSquared) {
                const float dist = std::sqrt(dist2);
                if (dist == 0) return;
                const Vector2F normal = vectorBetween / dist;
                float weightRatio = obj1.m_mass / (obj1.m_mass + obj2.m_mass);
                const float delta = m_collisionRestitution * (dist - engineDefaults::twoObjectsRadius);
                // Update positions
                if (obj1.m_isCollisionOn && obj2.m_isCollisionOn) {
                    if (!obj1.m_isPinned) obj1.m_posCurr -= normal * delta * (1 - weightRatio);
                    if (!obj2.m_isPinned) obj2.m_posCurr += normal * delta * weightRatio;
                    // TODO i should not call onCollision from here. because it is called from different threads. then removing other object from onCollision would be very risky
                    if constexpr (WithCallback) {
                        obj1.m_parent->v_onCollision(obj2.m_parent);
                        obj2.m_parent->v_onCollision(obj1.m_parent);
                    }
                }
            }
        };

        const int threadCount = m_threadPool.m_threadsNum;

        for (int i = 0; i < threadCount; i++) {
            m_threadPool.addTask([this, threadCount, &solveCont, i]() {
                m_grid.eachWithEachNeighbour(solveCont, threadCount, i, 0);
            });
        }
        m_threadPool.waitForCompletion();

        for (int i = 0; i < threadCount; i++) {
            m_threadPool.addTask([this, threadCount, &solveCont, i]() {
                m_grid.eachWithEachNeighbour(solveCont, threadCount, i, 1);
            });
        };
        m_threadPool.waitForCompletion();

        m_grid.eachWithEachNeighbour(solveCont, threadCount, threadCount);
    }

public:
    explicit Physics(Scene &scene, ThreadPool &threadPool, PerformanceMonitor &performanceMonitor)
            : m_scene(scene),
              m_grid(scene.m_grid),
              m_threadPool(threadPool), m_performanceMonitor(performanceMonitor) {}

    void update() {
        int localSubSteps = m_subSteps;
        bool localCollisionsEnabled = m_scene.getCollisionsEnabled();

        const float subStepDt = engineDefaults::physicsInterval / static_cast<float>(localSubSteps);

        for (int i = 0; i < localSubSteps; i++) {
            m_performanceMonitor.start("gravityConstraintsUpdate");
            updatePositionsConstraint(subStepDt);
            m_performanceMonitor.end("gravityConstraintsUpdate");

            m_performanceMonitor.start("m_grid");
            m_scene.rebuildGrid();
            m_performanceMonitor.end("m_grid");

            if (localCollisionsEnabled) {
                m_performanceMonitor.start("collisions");
                if (m_subStepsCallbackCounter % m_subStepsCallbackInterval == 0) {
                    solveCollisions<true>();
                } else {
                    solveCollisions<false>();
                }
                m_subStepsCallbackCounter++;
                m_performanceMonitor.end("collisions");

            }
            m_performanceMonitor.start("constrainSticks");
            constrainSticks();
            m_performanceMonitor.end("constrainSticks");
        }
    }

    // warning: if setting substeps from v_onCollision (for example), then m_subSteps number can change while update is running.
    // For that i use localSubSteps variable
    void setSubSteps(int value) {
        if (value > 0 && value < 16) {
            m_subSteps = value;
        } else {
            throw std::runtime_error("Substeps number should be between 1 and 16");
        }
    }

    [[nodiscard]] int getSubSteps() const {
        return m_subSteps;
    }


    // sets interval of how often onCollision callback will be called (in substeps)
    // if set to 1, then onCollision will be called on every substep
    // done to avoid calling onCollision too often (it's virtual method call, can be very slow)
    void setSubStepsCallbackInterval(int value) {
        m_subStepsCallbackInterval = value;
    }

    [[nodiscard]] int getSubStepsCallbackInterval() const {
        return m_subStepsCallbackInterval;
    }


    // adjusts how much m_objects will be "splitted" when resolving collisions.
    // 0 - not splitted no collision resolving happens. 1 - m_objects are fully splitted
    void setCollisionRestitution(float value) {
        if (value < 0 || value > 1) {
            throw std::runtime_error("Collision restitution should be between 0 and 1");
        }
        m_collisionRestitution = value;
    }

    [[nodiscard]] float getCollisionRestitution() const {
        return m_collisionRestitution;
    }

    // We multiply velocity of each object by this value on each substep.
    // Can help a bit if simulation goes to chaos
    // Value from 0 to 1
    void setLinearDamping(float value) {
        if (value < 0 || value > 1) {
            throw std::runtime_error("Linear damping should be between 0 and 1");
        }
        m_linearDamping = value;
    }

    [[nodiscard]] float getLinearDamping() const {
        return m_linearDamping;
    }

    // We multiply velocity of object by this value when it hits walls
    // Value from 0 to 1
    void setWallsDamping(float value) {
        if (value < 0 || value > 1) {
            throw std::runtime_error("Walls damping should be between 0 and 1");
        }
        m_wallsDamping = value;
    }

    [[nodiscard]] float getWallsDamping() const {
        return m_wallsDamping;
    }

    Physics(const Physics &) = delete;

    Physics(Physics &&) = delete;

    Physics &operator=(const Physics &) = delete;

    Physics &operator=(Physics &&) = delete;
};

//    void applyGravity() {
//        m_threadPool.dispatch(m_scene.getObjectsCount(), [this](int start, int end) {
//            m_scene.forEachStick([](BaseObject& object, int i) {
//                if (!object.m_isPinned) object.accelerate(m_gravity);
//            }, start, end);
//        });
//    }
//
//    void applyConstraints() {
//        const Rectangle bounds = m_scene.getSizeF();
//        const int m_objectsCount = m_scene.getObjectsCount();
//        m_threadPool.dispatch(m_objectsCount, [this, &bounds](int start, int end) {
//            m_scene.forEachStick([&bounds](BaseObject& object, int i) {
//                // problem was that for example: m_scene is 100x100. Then both m_objects are outside of field on same m_direction, like obj1(101.256, 102.399) and obj2(105.936, 110.87). both will be pushed to (100,100) resulting in zero distance.
//                // offset is trying to fix this problem
//                const float offset = static_cast<float>(i) * 1e-6f;
//
//                const Vector2F velocity = (object.m_posCurr - object.m_posOld) * m_wallsDamping;
//                if (object.m_posCurr.m_x < bounds.getWorldX1() + objectsRadius) {
//                    object.m_posCurr.m_x = bounds.getWorldX1() + objectsRadius + offset;
//                    object.m_posOld.m_x = object.m_posCurr.m_x + velocity.m_x;
//                } else if (object.m_posCurr.m_x > bounds.getX2() - objectsRadius) {
//                    object.m_posCurr.m_x = bounds.getX2() - objectsRadius - offset;
//                    object.m_posOld.m_x = object.m_posCurr.m_x + velocity.m_x;
//                }
//
//                if (object.m_posCurr.m_y < bounds.getWorldY1() + objectsRadius) {
//                    object.m_posCurr.m_y = bounds.getWorldY1() + objectsRadius + offset;
//                    object.m_posOld.m_y = object.m_posCurr.m_y + velocity.m_y;
//                } else if (object.m_posCurr.m_y > bounds.getY2() - objectsRadius) {
//                    object.m_posCurr.m_y = bounds.getY2() - objectsRadius - offset;
//                    object.m_posOld.m_y = object.m_posCurr.m_y + velocity.m_y;
//                }
//            }, start, end);
//        });
//    }
//    void updatePositions(float dt) {
//        m_threadPool.dispatch(m_scene.getObjectsCount(), [this, dt](int start, int end) {
//            m_scene.forEachStick([dt](BaseObject& object, int i) {
//                if (!object.m_isPinned) object.update(dt);
//            }, start, end);
//        });
//    }

//    void solveContact(BaseObject &obj1, BaseObject &obj2) {
//        Vector2F move = obj1.m_posCurr - obj2.m_posCurr;
//        const float dist2 = move.magnitude2();
//        const float min_dist = obj1.radius + obj2.radius;
//        // Check overlapping
//        if (dist2 < min_dist * min_dist) {
//            const float dist = sqrt(dist2);
//            move *= 0.5f * m_collisionRestitution * (dist - min_dist) / dist;
//            // Update positions
//            if (!obj1.m_isPinned) obj1.m_posCurr -= move;
//            if (!obj2.m_isPinned) obj2.m_posCurr += move;
//        }
//    }

//    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2) {
//        cell1.forEachId([&](int id1, int i){
//            cell2.forEachId([&](int id2, int i){
//                if (id1 == id2) return;
//                solveContact(m_scene.getObject(id1), m_scene.getObject(id2));
//            });
//        });
//    }

//    void rebuildGrid() {
//        m_grid.clear();
//        m_threadPool.dispatch(m_scene.getObjectsCount(), [this](int start, int end){
//            m_scene.forEachStick([this](BaseObject& object, int i) {
//                m_grid.insert(i, object.m_posCurr.m_x, object.m_posCurr.m_y);
//            }, start, end);
//        });
//    }