#pragma once

#include <vector>
#include <bits/shared_ptr.h>
#include <set>
#include "BaseObject/BaseObject.h"
#include "EngineConsts.h"
#include "utils/Grid.h"
#include "Camera.h"
#include "PerformanceMonitor.h"
#include "./utils/ThreadPool.h"
#include "utils/SparseSet.h"


class Scene {
private:
    std::vector<std::shared_ptr<BaseObject>> m_objects;
    std::set<int, std::greater<>> m_objectsToRemove;
    std::vector<int> m_objectsWithRotation;
    std::vector<BasicDetails> m_basicDetails;
    Vector2F m_sizeF;
    Vector2I m_sizeI;
    Camera m_camera;
    ThreadPool &m_threadPool;
    PerformanceMonitor &m_performanceMonitor;
    int m_maxObjectsNum;
    // collision m_grid when finding collisions is split on many threads (by columns). To avoid race conditions i use two pass method. This means that m_grid in fact will be split on numberOfThreads * 2 m_tasks. So, if we have 10threads
    // and m_grid of, say, size 39 will mean that each m_thread gets 1 m_task, and 19 remain for "cleanup" function, which is inefficient, because cleanup function (it solves remaining colums) runs on one core
    // Conclusion: best if m_grid is divided by threads * 2 without remainder
    int m_collisionGridWidth = m_sizeI.m_x / (engineDefaults::objectsRadius * 2);
    int m_collisionGridHeight = m_sizeI.m_y / (engineDefaults::objectsRadius * 2);

    void markObjectForRemoval(int index) {
        m_objectsToRemove.insert(index);
    }

    int getIndexByPtr(BaseObject *ptr) {
        auto it = m_objects.end();
        for (auto i = m_objects.begin(); i != m_objects.end(); i++) {
            if (i->get() == ptr) {
                it = i;
                break;
            }
        }
        if (it == m_objects.end()) {
            throw std::runtime_error("Trying to get index of ptr to object that does not exist");
        }
        return static_cast<int>(it - m_objects.begin());
    }

    int getIndexByPtr(std::weak_ptr<BaseObject> &ptr) {
        auto it = std::find(m_objects.begin(), m_objects.end(), ptr.lock());
        if (it == m_objects.end()) {
            throw std::runtime_error("Trying to get index of ptr to object that does not exist");
        }
        return static_cast<int>(it - m_objects.begin());
    }

public:

    IdGrid m_grid{m_collisionGridWidth, m_collisionGridHeight, getSizeI()};

    explicit Scene(float cameraMaxWorldViewSize, Vector2F cameraPosition, InputBus &inputBus, sf::RenderWindow &window,
                   ThreadPool &threadPool, PerformanceMonitor &performanceMonitor,
                   int maxObjectsNum, Vector2I size) :
            m_sizeF(Vector2F::fromOther(size)),
            m_sizeI(size),
            m_camera(cameraMaxWorldViewSize, cameraPosition, window, inputBus),
            m_threadPool(threadPool),
            m_performanceMonitor(performanceMonitor),
            m_maxObjectsNum(maxObjectsNum) {
        m_objects.reserve(maxObjectsNum);
        m_basicDetails.reserve(maxObjectsNum);
    }

    void toggleObjectRotation(BaseObject *ptr, bool enabled) {
        int index = getIndexByPtr(ptr);
        auto it = std::find(m_objectsWithRotation.begin(), m_objectsWithRotation.end(), index);
        if (it == m_objectsWithRotation.end() && enabled) {
            m_objectsWithRotation.push_back(index);
        } else if (!enabled) {
            m_objectsWithRotation.erase(it);
        }
    }

    template<typename T>
    std::weak_ptr<BaseObject> addObject(T &&object, Vector2F position) {
        if (m_objects.size() + 1 > m_maxObjectsNum) {
            throw std::runtime_error("Exceeded Scene capacity");
        }

        m_basicDetails.emplace_back(position);
        object.m_basicDetails = &m_basicDetails.back();

        std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<T>(object));
        m_objects.push_back(std::move(ptr));

        int index = static_cast<int>(m_objects.size()) - 1;
        m_basicDetails[index].m_parent = m_objects[index].get();
        m_objects[index]->v_onInit();
        return {m_objects[index]};
    }

    Camera &getCamera() {
        return m_camera;
    }

    // as i've discovered.. std::function HAS overhead. It was taking around 2 fps. So, i use T here. for performance-critical code parts better to not use std::function
    template<typename T>
    void forEachObject(const T &callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_objects.size());
        }

        for (int i = start; i < end; i++) {
            callback(*m_objects[i], i);
        }
    }

    template<typename T>
    void forEachBasicDetails(const T &callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_basicDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_basicDetails[i], i);
        }
    }

    template<typename T>
    void forEachBasicDetailsWithRotation(const T &callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_basicDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_basicDetails[m_objectsWithRotation[i]], i);
        }
    }


    BaseObject &getObject(int ind) {
        return *m_objects[ind];
    }

    BasicDetails &getBasicDetails(int ind) {
        return m_basicDetails[ind];
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(m_objects.size());
    }

    [[nodiscard]] int getObjectsWithRotationCount() const {
        return static_cast<int>(m_objectsWithRotation.size());
    }

    [[nodiscard]] Vector2F &getSizeF() {
        return m_sizeF;
    }

    [[nodiscard]] Vector2I &getSizeI() {
        return m_sizeI;
    }

    void runObjectTicks() {
        forEachObject([](BaseObject &object, int ind) {
            object.v_onTick();
        });
    }

    template<typename T>
    void forEachInRadius(Vector2F pos, float radius, const T &callback) {
        m_grid.forEachInRect(
                RectangleF::fromCoords(pos.m_x - radius, pos.m_y - radius, pos.m_x + radius, pos.m_y + radius),
                [&](int id) {
                    if ((m_basicDetails[id].m_posCurr - pos).magnitude2() < radius * radius) {
                        callback(m_basicDetails[id].m_parent, id);
                    }
                });
    }

    template<typename T>
    void lineTrace(Vector2F start, Vector2F end, const T &callback) {
        Vector2F lineVector = end - start;
        float lineLength = lineVector.magnitude();
        Vector2F lineVectorNormalized = lineVector / lineLength;

        m_grid.forEachAroundLine(start, end, [&](int id) {
            BasicDetails &basicDetails = m_basicDetails[id];
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

    // use std::set to have objectsToRemove in descending order
    // Because there's bad corner case:
    // if hypotetically vector is of size 100, and "removal set" is {15, 20, 99, 98),
    // then we would remove 15, swapped it with 99, popped, and element 99 would be at position 15. And removal at position 99 would fail
    void removeMarkedObjects() {
        if (m_objectsToRemove.empty()) {
            return;
        }

        // use swap & pop to remove objects without a lot of shifting
        for (const auto& i : m_objectsToRemove) {
            int lastIndex = getObjectsCount() - 1;
            std::swap(m_objects[i], m_objects[lastIndex]);
            std::swap(m_basicDetails[i], m_basicDetails[lastIndex]);
            m_objects.pop_back();
            m_basicDetails.pop_back();
            m_objects[i]->m_basicDetails = &m_basicDetails[i];
        }

        m_objectsToRemove.clear();
    }


    void removeObject(int index) {
        markObjectForRemoval(index);
    }

    void removeObject(std::weak_ptr<BaseObject> &ptr) {
        int index = getIndexByPtr(ptr);
        markObjectForRemoval(index);
    }

    // theres was bug: if you remove an object during m_physics calculations, it will cause some m_grid m_objects to become invalid (because everything is shifted), and m_grid uses id's
    // this can happen primarily if we for example remove an object in collision m_callback
    // to fix this we  use a stack of m_objects to remove, and remove them after m_physics calculations (and input handling)
    void removeObject(BaseObject *ptr) {
        int index = getIndexByPtr(ptr);
        markObjectForRemoval(index);
    }

    void rebuildGrid() {
        m_performanceMonitor.start("m_grid clear");
        m_threadPool.dispatch(m_grid.m_length, [this](int start, int end) {
            m_grid.clear(start, end);
        });
        m_performanceMonitor.end("m_grid clear");
        m_performanceMonitor.start("m_grid build");
        m_threadPool.dispatch(static_cast<int>(m_basicDetails.size()), [this](int start, int end) {
            for (int i = start; i < end; i++) {
                m_grid.insert(i, m_basicDetails[i].m_posCurr.m_x, m_basicDetails[i].m_posCurr.m_y);
            }
        });
//        forEachBasicDetails([this](BasicDetails &object, int i) {
//            m_grid.insert(i, object.m_posCurr.m_x, object.m_posCurr.m_y);
//        });
        m_performanceMonitor.end("m_grid build");
    }

    ObjectContext getObjectContext() {
        return ObjectContext{*this};
    };


    Scene(const Scene &) = delete;

    Scene(Scene &&) = delete;

    Scene &operator=(const Scene &) = delete;

    Scene &operator=(Scene &&) = delete;
};