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
    std::vector<std::shared_ptr<BaseObject>> objects;
    std::vector<BasicDetails> basicDetails;
    std::vector<int> objectsToRemove;
    Vector2F sizeF;
    Vector2I sizeI;
    Camera camera;
    ThreadPool& threadPool;
    PerformanceMonitor& performanceMonitor;
    int maxObjectsNum = engineDefaults::maxObjectsNum;
public:
    IdGrid grid{engineDefaults::collisionGridWidth, engineDefaults::collisionGridHeight, getSizeI()};

    explicit Scene(Vector2I size, Camera camera, ThreadPool& threadPool, PerformanceMonitor& performanceMonitor, int maxObjectsNum) : sizeF(Vector2F::fromOther(size)), sizeI(size), camera(camera), threadPool(threadPool), performanceMonitor(performanceMonitor), maxObjectsNum(maxObjectsNum) {
        objects.reserve(maxObjectsNum);
        basicDetails.reserve(maxObjectsNum);
        objectsToRemove.reserve(maxObjectsNum);
    }

    template<typename T>
    std::weak_ptr<BaseObject> addObject(T &&object, Vector2F position) {
        if (objects.size() >= maxObjectsNum) {
            throw std::runtime_error("Exceeded Scene capacity");
        }

        basicDetails.emplace_back(position);
        object.basicDetails = &basicDetails.back();

        std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<T>(object));
        objects.push_back(std::move(ptr));

        int index = static_cast<int>(objects.size()) - 1;
        basicDetails[index].parent = objects[index].get();
        objects[index]->onInit();
        return {objects[index]};
    }

    Camera& getCamera() {
        return camera;
    }


    template<typename Func>
    void forEachObject(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(objects.size());
        }

        for (int i = start; i < end; i++) {
            callback(*objects[i], i);
        }
    }

    template<typename Func>
    void forEachBasicDetails(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(basicDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(basicDetails[i], i);
        }
    }


    BaseObject &getObject(int ind) {
        return *objects[ind];
    }

    BasicDetails &getBasicDetails(int ind) {
        return basicDetails[ind];
    }

    void clear() {
        objects.clear();
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(objects.size());
    }

    [[nodiscard]] Vector2F &getSizeF() {
        return sizeF;
    }

    [[nodiscard]] Vector2I &getSizeI() {
        return sizeI;
    }

    void removeObjects() {
        objects.clear();
    }

    void runTick() {
        forEachObject([](BaseObject &object, int ind) {
            object.onTick();
        });
    }

    // TODO not copy callback
    void forEachInRadius(Vector2F pos, float radius, std::function<void(BaseObject*, int)> callback) {
//        forEachBasicDetails([&](BasicDetails &details, int ind) {
//            if ((details.posCurr - pos).magnitude2() < radius * radius) {
//                callback(details.parent, ind);
//            }
//        });
        grid.forEachInRect(RectangleF::fromCoords(pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius), [&](int id) {
            if ((basicDetails[id].posCurr - pos).magnitude2() < radius * radius) {
                callback(basicDetails[id].parent, id);
            }
        });
    }


    void removeMarkedObjects() {
        // need to sort objectsToRemove in descending order

        if (objectsToRemove.empty()) {
            return;
        }
        std::sort(objectsToRemove.begin(), objectsToRemove.end(), std::greater<>());

        for (int i : objectsToRemove) {
            objects.erase(objects.begin() + i);
            basicDetails.erase(basicDetails.begin() + i);
        }

        for (int i = 0; i < objects.size(); i++) {
            objects[i]->basicDetails = &basicDetails[i];
        }
        objectsToRemove.clear();
    }

    void markObjectForRemoval(int index) {
        if (std::find(objectsToRemove.begin(), objectsToRemove.end(), index) == objectsToRemove.end()) {
            objectsToRemove.push_back(index);
        }
    }


    void removeObject(int index) {
        markObjectForRemoval(index);
    }

    void removeObject(std::weak_ptr<BaseObject> &ptr) {
        auto it = std::find(objects.begin(), objects.end(), ptr.lock());
        if (it != objects.end()) {
            int index = static_cast<int>(it - objects.begin());
            markObjectForRemoval(index);
        }
    }

    // theres was bug: if you remove an object during physics calculations, it will cause some grid objects to become invalid (because everything is shifted), and grid uses id's
    // this can happen primarily if we for example remove an object in collision callback
    // to fix this we  use a stack of objects to remove, and remove them after physics calculations (and input handling)
    void removeObject(BaseObject *ptr) {
        auto it = objects.end();
        for (auto i = objects.begin(); i != objects.end(); i++) {
            if (i->get() == ptr) {
                it = i;
                break;
            }
        }
        if (it != objects.end()) {
            int index = static_cast<int>(it - objects.begin());
            markObjectForRemoval(index);
        }
    }
    void rebuildGrid() {
        performanceMonitor.start("grid clear");
        threadPool.dispatch(grid.length, [this](int start, int end) {
            grid.clear(start, end);
        });
        performanceMonitor.end("grid clear");
        performanceMonitor.start("grid build");
        forEachBasicDetails([this](BasicDetails &object, int i) {
            grid.insert(i, object.posCurr.x, object.posCurr.y);
        });
        performanceMonitor.end("grid build");
    }
};