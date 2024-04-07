#pragma once

#include <vector>
#include <bits/shared_ptr.h>
#include <set>
#include <typeindex>
#include "../BaseObject/BaseObject.h"
#include "../EngineConsts.h"
#include "../utils/Grid.h"
#include "../Camera.h"
#include "../PerformanceMonitor.h"
#include "../utils/ThreadPool.h"
#include "../utils/SparseSet.h"
#include "ObjectStorage.h"
#include "StickStorage.h"

enum BOUNDARY_TYPE {
    SOLID = 10,
    WRAP = 11,
};

class Scene {
private:
    ObjectStorage m_os;
    StickStorage m_stickStorage;

    Vector2F m_sizeF;
    Vector2I m_sizeI;

    Camera m_camera;

    ThreadPool &m_threadPool;

    PerformanceMonitor &m_performanceMonitor;
    BOUNDARY_TYPE m_boundaryType = BOUNDARY_TYPE::SOLID;

    Vector2F m_gravity = engineDefaults::gravity;

    float m_maxVelocity = engineDefaults::maxVelocity;
    bool m_collisionsEnabled = true;

    // collision m_grid when finding collisions is split on many threads (by columns). To avoid race conditions i use two pass method. This means that m_grid in fact will be split on numberOfThreads * 2 m_tasks. So, if we have 10threads
    // and m_grid of, say, size 39 will mean that each m_thread gets 1 m_task, and 19 remain for "cleanup" function, which is inefficient, because cleanup function (it solves remaining colums) runs on one core
    // Conclusion: best if m_grid is divided by threads * 2 without remainder
    int m_collisionGridWidth = m_sizeI.m_x / (engineDefaults::objectsRadius * 2);
    int m_collisionGridHeight = m_sizeI.m_y / (engineDefaults::objectsRadius * 2);
public:

    IdGrid m_grid{m_collisionGridWidth, m_collisionGridHeight, getSizeI()};

    explicit Scene(float cameraMaxWorldViewSize, Vector2F cameraPosition, InputBus &inputBus, sf::RenderWindow &window,
                   ThreadPool &threadPool, PerformanceMonitor &performanceMonitor,
                   int maxObjectsNum, Vector2I size) :
            m_os(maxObjectsNum, this),
            m_stickStorage(maxObjectsNum),
            m_sizeF(Vector2F::fromOther(size)),
            m_sizeI(size),
            m_camera(cameraMaxWorldViewSize, cameraPosition, window, inputBus),
            m_threadPool(threadPool),
            m_performanceMonitor(performanceMonitor) {
    }

    ObjectStorage &getObjectStorage() {
        return m_os;
    }

    StickStorage &getStickStorage() {
        return m_stickStorage;
    }

    Camera &getCamera() {
        return m_camera;
    }

    [[nodiscard]] Vector2F &getSizeF() {
        return m_sizeF;
    }

    [[nodiscard]] Vector2I &getSizeI() {
        return m_sizeI;
    }

    void runTicks() {
        m_os.forEachObject([](BaseObject &object, int ind) {
            object.v_onTick();
        });
    }

    template<typename T>
    void forEachInRadius(Vector2F pos, float radius, const T &callback) {
        m_grid.forEachInRect(
                RectangleF::fromCoords(pos.m_x - radius, pos.m_y - radius, pos.m_x + radius, pos.m_y + radius),
                [&](int id) {
                    if ((m_os.getBasicDetails(id).m_posCurr - pos).magnitude2() < radius * radius) {
                        callback(m_os.getBasicDetails(id).m_parent, id);
                    }
                });
    }

    template<typename T>
    void forEachInSquare(Vector2F pos, float radius, const T &callback) {
        m_grid.forEachInRect(
                RectangleF::fromCoords(pos.m_x - radius, pos.m_y - radius, pos.m_x + radius, pos.m_y + radius),
                [&](int id) {
                    callback(m_os.getBasicDetails(id).m_parent, id);
                });
    }


    template<typename T>
    void lineTrace(Vector2F start, Vector2F end, const T &callback) {
        Vector2F lineVector = end - start;
        float lineLength = lineVector.magnitude();
        Vector2F lineVectorNormalized = lineVector / lineLength;

        m_grid.forEachAroundLine(start, end, [&](int id) {
            BasicDetails &basicDetails = m_os.getBasicDetails(id);
            Vector2F startToObject = basicDetails.m_posCurr - start;
            float projectionLength = startToObject.dot(lineVectorNormalized);
            if (projectionLength >= 0 && projectionLength < lineLength) {
                Vector2F centerProjectionOnRay = start + lineVectorNormalized * projectionLength;
                float centerToCenterProjectionMagnitude2 = (basicDetails.m_posCurr -
                                                            centerProjectionOnRay).magnitude2();
                bool isInside = centerToCenterProjectionMagnitude2 < engineDefaults::objectsRadiusSquared;
                if (isInside) {
                    callback(basicDetails.m_parent, id);
                }
            }
        });

//        m_grid.forEachAroundLine(start, end, [&](int id) {
//            BasicDetails &basicDetails = m_basicDetails[id];
//            Vector2F startToObject = basicDetails.m_posCurr - start;
//            float startToObjectLengthSquared = startToObject.magnitude2();
//            float projectionLength = startToObject.dot(lineVectorNormalized);
//            if (projectionLength >= 0 && projectionLength < lineLength) {
//                float projectionLengthSquared = projectionLength * projectionLength;
//                bool isInside = startToObjectLengthSquared - projectionLengthSquared < engineDefaults::objectsRadiusSquared;
//                if (isInside) {
//                    callback(basicDetails.m_parent, id);
//                }
//            }
//        });

//        m_grid.forEachAroundLine(start, end, [&](int id) {
//            BasicDetails &basicDetails = m_basicDetails[id];
//            callback(basicDetails.m_parent, id);
//        });
    }


    void rebuildGrid() {
        m_performanceMonitor.start("m_grid clear");
        int threadCount = m_threadPool.m_threadsNum;
        for (int i = 0; i < threadCount; i++) {
            m_threadPool.addTask([this, threadCount, i]() {
                m_grid.clear(threadCount, i);
            });
        }
        m_threadPool.waitForCompletion();
        m_grid.clear(threadCount, threadCount);

        m_performanceMonitor.end("m_grid clear");
        m_performanceMonitor.start("m_grid build");
        m_threadPool.dispatch(static_cast<int>(m_os.getObjectsCount()), [this](int start, int end) {
            for (int i = start; i < end; i++) {
                m_grid.insert(i, m_os.getBasicDetails(i).m_posCurr.m_x, m_os.getBasicDetails(i).m_posCurr.m_y);
            }
        });
//        forEachBasicDetails([this](BasicDetails &object, int i) {
//            m_grid.insert(i, object.m_posCurr.m_x, object.m_posCurr.m_y);
//        });
        m_performanceMonitor.end("m_grid build");
    }

    Vector2F &getGravity() {
        return m_gravity;
    }

    // set gravity.
    // high values can cause m_objects to pass through each other and other weird stuff
    void setGravity(Vector2F gravity) {
        m_gravity = gravity;
    }

    // limit velocity of each object on each update() call.
    // This can prevent full chaos.
    void setMaxVelocity(float value) {
        m_maxVelocity = value;
    }

    [[nodiscard]] float getMaxVelocity() const {
        return m_maxVelocity;
    }

    void setCollisionsEnabled(bool enabled) {
        m_collisionsEnabled = enabled;
    }

    [[nodiscard]] bool getCollisionsEnabled() const {
        return m_collisionsEnabled;
    }

    void setBoundaryType(BOUNDARY_TYPE boundaryType) {
        m_boundaryType = boundaryType;
    }

    [[nodiscard]] BOUNDARY_TYPE getBoundaryType() const {
        return m_boundaryType;
    }

    [[nodiscard]] float getDistanceToTop(Vector2F pos) const {
        return pos.m_y - 0;
    }

    [[nodiscard]] float getDistanceToBottom(Vector2F pos) const {
        return m_sizeF.m_y - pos.m_y;
    }

    [[nodiscard]] float getDistanceToLeft(Vector2F pos) const {
        return pos.m_x - 0;
    }

    [[nodiscard]] float getDistanceToRight(Vector2F pos) const {
        return m_sizeF.m_x - pos.m_x;
    }

    Scene(const Scene &) = delete;

    Scene(Scene &&) = delete;

    Scene &operator=(const Scene &) = delete;

    Scene &operator=(Scene &&) = delete;
};