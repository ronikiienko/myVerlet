#pragma once

#include <vector>
#include <bits/shared_ptr.h>
#include "BaseObject.h"
#include "EngineConsts.h"
#include "utils/Grid.h"
#include "Camera.h"
#include "PerformanceMonitor.h"


class Scene {
private:
    std::vector<std::shared_ptr<BaseObject>> m_objects;
    std::vector<BasicDetails> m_basicDetails;
    std::vector<int> m_objectsToRemove;
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
public:
    IdGrid grid{m_collisionGridWidth, m_collisionGridHeight, getSizeI()};

    explicit Scene(float cameraMaxWorldViewSize, Vector2F cameraPosition, InputHandler& inputHandler, sf::RenderWindow& window,  ThreadPool &threadPool, PerformanceMonitor &performanceMonitor,
                   int maxObjectsNum, Vector2I size) :
            m_sizeF(Vector2F::fromOther(size)),
            m_sizeI(size),
            m_camera(cameraMaxWorldViewSize, cameraPosition, window, inputHandler),
            m_threadPool(threadPool),
            m_performanceMonitor(performanceMonitor),
            m_maxObjectsNum(maxObjectsNum) {
        m_objects.reserve(maxObjectsNum);
        m_basicDetails.reserve(maxObjectsNum);
        m_objectsToRemove.reserve(maxObjectsNum);
    }

    template<typename T>
    std::weak_ptr<BaseObject> addObject(T &&object, Vector2F position) {
        if (m_objects.size() >= m_maxObjectsNum) {
            throw std::runtime_error("Exceeded Scene capacity");
        }

        m_basicDetails.emplace_back(position);
        object.m_basicDetails = &m_basicDetails.back();

        std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<T>(object));
        m_objects.push_back(std::move(ptr));

        int index = static_cast<int>(m_objects.size()) - 1;
        m_basicDetails[index].m_parent = m_objects[index].get();
        m_objects[index]->onInit();
        return {m_objects[index]};
    }

    Camera &getCamera() {
        return m_camera;
    }


    template<typename Func>
    void forEachObject(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_objects.size());
        }

        for (int i = start; i < end; i++) {
            callback(*m_objects[i], i);
        }
    }

    template<typename Func>
    void forEachBasicDetails(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_basicDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_basicDetails[i], i);
        }
    }


    BaseObject &getObject(int ind) {
        return *m_objects[ind];
    }

    BasicDetails &getBasicDetails(int ind) {
        return m_basicDetails[ind];
    }

    void clear() {
        m_objects.clear();
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(m_objects.size());
    }

    [[nodiscard]] Vector2F &getSizeF() {
        return m_sizeF;
    }

    [[nodiscard]] Vector2I &getSizeI() {
        return m_sizeI;
    }

    void removeObjects() {
        m_objects.clear();
    }

    void runTick() {
        forEachObject([](BaseObject &object, int ind) {
            object.onTick();
        });
    }

    // TODO not copy m_callback
    void forEachInRadius(Vector2F pos, float radius, std::function<void(BaseObject *, int)> callback) {
//        forEachBasicDetails([&](BasicDetails &details, int ind) {
//            if ((details.m_posCurr - pos).magnitude2() < radius * radius) {
//                m_callback(details.m_parent, ind);
//            }
//        });
        grid.forEachInRect(RectangleF::fromCoords(pos.m_x - radius, pos.m_y - radius, pos.m_x + radius, pos.m_y + radius),
                           [&](int id) {
                               if ((m_basicDetails[id].m_posCurr - pos).magnitude2() < radius * radius) {
                                   callback(m_basicDetails[id].m_parent, id);
                               }
                           });
    }


    void removeMarkedObjects() {
        // need to sort m_objectsToRemove in descending order

        if (m_objectsToRemove.empty()) {
            return;
        }
        std::sort(m_objectsToRemove.begin(), m_objectsToRemove.end(), std::greater<>());

        for (int i: m_objectsToRemove) {
            m_objects.erase(m_objects.begin() + i);
            m_basicDetails.erase(m_basicDetails.begin() + i);
        }

        for (int i = 0; i < m_objects.size(); i++) {
            m_objects[i]->m_basicDetails = &m_basicDetails[i];
        }
        m_objectsToRemove.clear();
    }

    void markObjectForRemoval(int index) {
        if (std::find(m_objectsToRemove.begin(), m_objectsToRemove.end(), index) == m_objectsToRemove.end()) {
            m_objectsToRemove.push_back(index);
        }
    }


    void removeObject(int index) {
        markObjectForRemoval(index);
    }

    void removeObject(std::weak_ptr<BaseObject> &ptr) {
        auto it = std::find(m_objects.begin(), m_objects.end(), ptr.lock());
        if (it != m_objects.end()) {
            int index = static_cast<int>(it - m_objects.begin());
            markObjectForRemoval(index);
        }
    }

    // theres was bug: if you remove an object during m_physics calculations, it will cause some m_grid m_objects to become invalid (because everything is shifted), and m_grid uses id's
    // this can happen primarily if we for example remove an object in collision m_callback
    // to fix this we  use a stack of m_objects to remove, and remove them after m_physics calculations (and input handling)
    void removeObject(BaseObject *ptr) {
        auto it = m_objects.end();
        for (auto i = m_objects.begin(); i != m_objects.end(); i++) {
            if (i->get() == ptr) {
                it = i;
                break;
            }
        }
        if (it != m_objects.end()) {
            int index = static_cast<int>(it - m_objects.begin());
            markObjectForRemoval(index);
        }
    }

    void rebuildGrid() {
        m_performanceMonitor.start("m_grid clear");
        m_threadPool.dispatch(grid.m_length, [this](int start, int end) {
            grid.clear(start, end);
        });
        m_performanceMonitor.end("m_grid clear");
        m_performanceMonitor.start("m_grid build");
        forEachBasicDetails([this](BasicDetails &object, int i) {
            grid.insert(i, object.m_posCurr.m_x, object.m_posCurr.m_y);
        });
        m_performanceMonitor.end("m_grid build");
    }

    Scene(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&) = delete;
};