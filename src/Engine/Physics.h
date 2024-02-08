#pragma once

#include "Scene.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "utils/Grid.h"
#include "PerformanceMonitor.h"
#include "EngineConsts.h"

class Physics {
private:
    Scene &m_scene;
    IdGrid& m_grid;
    ThreadPool &m_threadPool;
    PerformanceMonitor &m_performanceMonitor;

    int m_subSteps = engineDefaults::physicsSubSteps;
    bool m_collisionsEnabled = true;
    float m_maxVelocity = engineDefaults::maxVelocity;
    Vector2F m_gravity = engineDefaults::gravity;
    float m_collisionRestitution = engineDefaults::collisionRestitution;
    float m_linearDamping = engineDefaults::linearDamping;
    float m_wallsDamping = engineDefaults::wallsDamping;


    void updatePositionsConstraint(float dt) {
        const Vector2F size = m_scene.getSizeF();
        const int objectsCount = m_scene.getObjectsCount();
        m_threadPool.dispatch(objectsCount, [this, &size, dt](int start, int end) {
            const float minX = 0 + engineDefaults::objectsRadius;
            const float maxX = size.m_x - engineDefaults::objectsRadius;
            const float minY = 0 + engineDefaults::objectsRadius;
            const float maxY = size.m_y - engineDefaults::objectsRadius;
            m_scene.forEachBasicDetails([&size, dt, minX, maxX, minY, maxY, this](BasicDetails &object, int i) {
                // TODO when all m_grid filled with m_objects, you can see that some start falling faster and some slower (on lower m_gravity levels like 10) - this is because of floats precision
                if (!object.m_isPinned) {
                    object.accelerate(m_gravity);
//                    object.update(dt);
                    Vector2F velocity = object.m_posCurr - object.m_posOld;
                    velocity *= m_linearDamping;
                    // TODO review maby limiting not needed
                    velocity.limitMagnitude(m_maxVelocity);

                    object.m_posOld = object.m_posCurr;
                    object.m_posCurr += velocity + (object.m_acceleration * dt);

                    object.m_acceleration = Vector2F::cart();
                }

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

            }, start, end);
        });
    }

    template<bool WithCallback>
    void solveContact(BasicDetails &obj1, BasicDetails &obj2) {
        const Vector2F vectorBetween = obj1.m_posCurr - obj2.m_posCurr;
        const float dist2 = vectorBetween.magnitude2();
        // Check overlapping
        if (dist2 < engineDefaults::twoObjectsRadiusSquared) {


            const float dist = std::sqrt(dist2);
            if (dist == 0) return;
            const Vector2F normal = vectorBetween / dist;
            const float delta = 0.5f * m_collisionRestitution * (dist - engineDefaults::twoObjectsRadius);
            // Update positions
            if (obj1.m_isCollisionOn && obj2.m_isCollisionOn) {
                obj1.m_posCurr -= normal * delta;
                obj2.m_posCurr += normal * delta;
                // TODO i should not call onCollision from here. because it is called from different threads. then removing other object from onCollision would be very risky
                if constexpr (WithCallback) {
                    obj1.m_collidedWith = obj2.m_parent;
                    obj2.m_collidedWith = obj1.m_parent;
                }
            }
        }
    }

    template<bool WithCallback>
    void solveCollisionsTwoCells(const Cell &cell1, const Cell &cell2) {
        for (int i = 0; i < cell1.activeCount; i++) {
            const int id1 = cell1.ids[i];
            for (int j = 0; j < cell2.activeCount; j++) {
                const int id2 = cell2.ids[j];
                if (id1 == id2) continue;
                solveContact<WithCallback>(m_scene.getBasicDetails(id1), m_scene.getBasicDetails(id2));
            }
        }
    }

    template<bool WithCallback>
    void solveCollisionsSubgrid(int startX, int endX, int startY, int endY) {
        for (int i = startX; i < endX; i++) {
            for (int j = startY; j < endY; j++) {
                const Cell &cell1 = m_grid.get(i, j);
                solveCollisionsTwoCells<WithCallback>(cell1, cell1);
                if (i + 1 < m_grid.m_width && j - 1 >= 0) {
                    solveCollisionsTwoCells<WithCallback>(cell1, m_grid.get(i + 1, j - 1));
                }
                if (i + 1 < m_grid.m_width) {
                    solveCollisionsTwoCells<WithCallback>(cell1, m_grid.get(i + 1, j));
                }
                if (i + 1 < m_grid.m_width && j + 1 < m_grid.m_height) {
                    solveCollisionsTwoCells<WithCallback>(cell1, m_grid.get(i + 1, j + 1));
                }
                if (j + 1 < m_grid.m_height) {
                    solveCollisionsTwoCells<WithCallback>(cell1, m_grid.get(i, j + 1));
                }
            }
        }
    }

    template<bool WithCallback>
    void solveCollisions() {
        const int threadCount = m_threadPool.m_threadsNum;
        const int sliceCount = threadCount * 2;
        const int sliceSize = m_grid.m_width / sliceCount;
        //  to avoid m_data races - process in two passes. So that no threads are assigned to neighbouring columns
        for (int i = 0; i < threadCount; i++) {
            m_threadPool.addTask([i, sliceSize, this]() {
                int startCol = (i * 2) * sliceSize;
                int endCol = startCol + sliceSize;
                solveCollisionsSubgrid<WithCallback>(startCol, endCol, 0, m_grid.m_height);
            });
        }
        m_threadPool.waitForCompletion();

        for (int i = 0; i < threadCount; i++) {
            m_threadPool.addTask([i, sliceSize, this]() {
                int startCol = (i * 2 + 1) * sliceSize;
                int endCol = startCol + sliceSize;
                solveCollisionsSubgrid<WithCallback>(startCol, endCol, 0, m_grid.m_height);
            });
        };
        m_threadPool.waitForCompletion();

        if (sliceSize * sliceCount < m_grid.m_width) {
            m_threadPool.addTask([sliceSize, sliceCount, this]() {
                int startCol = sliceSize * sliceCount;
                int endCol = m_grid.m_width;
                solveCollisionsSubgrid<WithCallback>(startCol, endCol, 0, m_grid.m_height);
            });
        }
        m_threadPool.waitForCompletion();
    }
public:
    explicit Physics(Scene &scene, ThreadPool &threadPool, PerformanceMonitor &performanceMonitor)
            : m_scene(scene),
              m_grid(scene.m_grid),
              m_threadPool(threadPool), m_performanceMonitor(performanceMonitor) {}

    void update() {
        int localSubSteps = m_subSteps;
        bool localCollisionsEnabled = m_collisionsEnabled;

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
                if (i == localSubSteps - 1) {
                    solveCollisions<true>();
                } else {
                    solveCollisions<false>();
                }
                m_performanceMonitor.end("collisions");
            }
        }
    }

    // First, i created onCollision virtual methods in all BaseObjects. Then from physics i called them.
    // But this had two problems: 1. they were called from different threads. 2. They were virtual, so calling them was very expensive
    // To solve that, i created m_collidedWith pointer in BasicDetails. Then from physics i set it to collided object.
    // Then from onTick i can do whatever i want with it.
    // And after frame ends, i can reset all pointers so that one collision is not handled multiple times
    void removeCollisionRecords() {
        m_threadPool.dispatch(m_scene.getObjectsCount(), [this](int start, int end) {
            m_scene.forEachBasicDetails([](BasicDetails &object, int i) {
                object.m_collidedWith = nullptr;
            }, start, end);
        });
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

    void setCollisionsEnabled(bool enabled) {
        m_collisionsEnabled = enabled;
    }

    [[nodiscard]] bool getCollisionsEnabled() const {
        return m_collisionsEnabled;
    }

    // limit velocity of each object on each update() call.
    // This can prevent full chaos.
    void setMaxVelocity(float value) {
        m_maxVelocity = value;
    }

    [[nodiscard]] float getMaxVelocity() const {
        return m_maxVelocity;
    }

    // set m_gravity.
    // high values can cause m_objects to pass through each other and other weird stuff
    void setGravity(Vector2F value) {
        m_gravity = value;
    }

    [[nodiscard]] Vector2F getGravity() const {
        return m_gravity;
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

    Physics(const Physics&) = delete;
    Physics(Physics&&) = delete;
    Physics& operator=(const Physics&) = delete;
    Physics& operator=(Physics&&) = delete;
};

//    void applyGravity() {
//        m_threadPool.dispatch(m_scene.getObjectsCount(), [this](int start, int end) {
//            m_scene.forEachObject([](BaseObject& object, int i) {
//                if (!object.m_isPinned) object.accelerate(m_gravity);
//            }, start, end);
//        });
//    }
//
//    void applyConstraints() {
//        const Rectangle bounds = m_scene.getSizeF();
//        const int m_objectsCount = m_scene.getObjectsCount();
//        m_threadPool.dispatch(m_objectsCount, [this, &bounds](int start, int end) {
//            m_scene.forEachObject([&bounds](BaseObject& object, int i) {
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
//            m_scene.forEachObject([dt](BaseObject& object, int i) {
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
//            m_scene.forEachObject([this](BaseObject& object, int i) {
//                m_grid.insert(i, object.m_posCurr.m_x, object.m_posCurr.m_y);
//            }, start, end);
//        });
//    }