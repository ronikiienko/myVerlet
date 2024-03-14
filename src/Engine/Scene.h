#pragma once

#include <vector>
#include <bits/shared_ptr.h>
#include <set>
#include <typeindex>
#include "BaseObject/BaseObject.h"
#include "EngineConsts.h"
#include "utils/Grid.h"
#include "Camera.h"
#include "PerformanceMonitor.h"
#include "./utils/ThreadPool.h"
#include "utils/SparseSet.h"

enum BOUNDARY_TYPE {
    SOLID = 10,
    WRAP = 11,
};

class Scene {
private:
    int m_maxObjectsNum;
    std::vector<std::shared_ptr<BaseObject>> m_objects;
    std::set<int, std::greater<>> m_objectsToRemove;
    SparseSet m_objectsWithRotation;
    std::vector<BasicDetails> m_basicDetails;
    std::unordered_map<std::type_index, std::set<int>> m_objectTypesIndexLists;

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
            m_maxObjectsNum(maxObjectsNum),
            m_objectsWithRotation(maxObjectsNum) {
        m_objects.reserve(maxObjectsNum);
        m_basicDetails.reserve(maxObjectsNum);
    }

    void toggleObjectRotation(BaseObject *ptr, bool enabled) {
        int index = getIndexByPtr(ptr);
        if (enabled) {
            m_objectsWithRotation.insert(index);
        } else {
            m_objectsWithRotation.remove(index);
        }
    }

    template<typename T>
    std::weak_ptr<BaseObject> addObject(T &&object, Vector2F position) {
        if (m_objects.size() + 1 > m_maxObjectsNum) {
            throw std::runtime_error("Exceeded Scene capacity");
        }

        m_basicDetails.emplace_back(position);
        object.m_basicDetails = &m_basicDetails.back();

        m_objects.push_back(std::make_shared<T>(std::forward<T>(object)));

        int index = static_cast<int>(m_objects.size()) - 1;
        m_basicDetails[index].m_parent = m_objects[index].get();
        m_objects[index]->v_onInit();
        m_objectTypesIndexLists[typeid(T)].insert(index);
        return {m_objects[index]};
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(m_objects.size());
    }

    // use std::set to have objectsToRemove in descending order
    // Because there's bad corner case:
    // if hypotetically vector is of size 100, and "removal set" is {15, 20, 99, 98},
    // then we would remove 15, swapped it with 99, popped, and element 99 would be at position 15. And removal at position 99 would fail
    void removeMarkedObjects() {
        if (m_objectsToRemove.empty()) {
            return;
        }

        // use swap & pop to remove objects without a lot of shifting
        for (const auto &i: m_objectsToRemove) {
            int lastIndex = getObjectsCount() - 1;
            if (i > lastIndex || i < 0) {
                throw std::runtime_error("Trying to remove object that does not exist");
            }

            if (i == lastIndex) {
                // this actually works. typeid(removedObjectRef) returns the type of the derived object, not BaseObject
                // be cautious, that if I would use typeid directly on pointer, it would return BaseObject for any derived object
                BaseObject& removedObjectRef = *(m_objects[i].get());
                m_objectTypesIndexLists[typeid(removedObjectRef)].erase(i);

                m_objectsWithRotation.remove(i);
                m_objects.pop_back();
                m_basicDetails.pop_back();
            }
            if (i < lastIndex) {
                BaseObject& removedObjectRef = *m_objects[i];
                BaseObject& lastObjectRef = *m_objects[lastIndex];
                std::type_index removedTypeIndex = typeid(removedObjectRef);
                std::type_index lastTypeIndex = typeid(lastObjectRef);
                m_objectTypesIndexLists[removedTypeIndex].erase(i);
                m_objectTypesIndexLists[lastTypeIndex].erase(lastIndex);
                m_objectTypesIndexLists[lastTypeIndex].insert(i);

                if (m_objectsWithRotation.contains(lastIndex)) {
                    m_objectsWithRotation.insert(i);
                    m_objectsWithRotation.remove(lastIndex);
                } else {
                    m_objectsWithRotation.remove(i);
                }
                std::swap(m_objects[i], m_objects[lastIndex]);
                std::swap(m_basicDetails[i], m_basicDetails[lastIndex]);
                m_objects.pop_back();
                m_basicDetails.pop_back();
                m_objects[i]->m_basicDetails = &m_basicDetails[i];
            }
        }

        m_objectsToRemove.clear();
    }

    // Important node: removal itself happens in the end of the frame. After all physics substeps and input handling.
    // So, for example, checking weakPointer.expired() in the same frame after removeObject() will return false
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

    // function is pretty fast. It uses static_cast for casting.
    // And internal list of indices for each type of object
    // But can be improved by using std::vector instead of std::set,
    // and storing pointers instead of indices to avoid indirection
    template<typename ObjectType, typename T>
    void forEachObjectOfType(const T &callback) {
        for (int i: m_objectTypesIndexLists[typeid(ObjectType)]) {
            callback(static_cast<ObjectType&>(*m_objects[i]), i);
        }
    }

    // this fast, as it doesn't actually count anything in loop, but just get's size of set
    template<typename ObjectType>
    int getObjectsOfTypeCount() {
        return static_cast<int>(m_objectTypesIndexLists[typeid(ObjectType)].size());
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
        m_objectsWithRotation.forEach([&](int id, int iteration) {
            callback(m_basicDetails[id], id, iteration);
        }, start, end);
    }


    BaseObject &getObject(int ind) {
        return *m_objects[ind];
    }

    BasicDetails &getBasicDetails(int ind) {
        return m_basicDetails[ind];
    }



    [[nodiscard]] int getObjectsWithRotationCount() const {
        return m_objectsWithRotation.size();
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

    void logDifferentTypeCounts() {
        for (const auto &i: m_objectTypesIndexLists) {
            std::cout << i.first.name() << " " << i.second.size() << std::endl;
        }
    }

    Scene(const Scene &) = delete;

    Scene(Scene &&) = delete;

    Scene &operator=(const Scene &) = delete;

    Scene &operator=(Scene &&) = delete;
};