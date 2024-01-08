#pragma once

#include <vector>
#include <bits/shared_ptr.h>
#include "BaseObject.h"
#include "EngineConsts.h"
#include "utils/Grid.h"


class AtomWorld {
private:
    std::vector<std::shared_ptr<BaseObject>> objects;
    std::vector<BasicDetails> basicDetails;
    std::vector<int> objectsToRemove;
    RectangleF boundsF;
    RectangleI boundsI;
public:
    IdGrid grid{consts::collisionGridWidth, consts::collisionGridHeight, getBoundsI()};

    explicit AtomWorld(RectangleI bounds) : boundsF(RectangleF::fromOther(bounds)), boundsI(bounds) {
        objects.reserve(consts::maxObjectNum);
        basicDetails.reserve(consts::maxObjectNum);
        objectsToRemove.reserve(consts::maxObjectNum);
    }

    template<typename T>
    std::weak_ptr<BaseObject> addObject(T &&object, Vector2 position) {
        basicDetails.emplace_back(position);
        object.basicDetails = &basicDetails.back();

        std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<T>(object));
        objects.push_back(std::move(ptr));

        int index = static_cast<int>(objects.size()) - 1;
        basicDetails[index].parent = objects[index].get();
        objects[index]->onInit();
        return {objects[index]};
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

    [[nodiscard]] RectangleF &getBoundsF() {
        return boundsF;
    }

    [[nodiscard]] RectangleI &getBoundsI() {
        return boundsI;
    }

    void removeObjects() {
        objects.clear();
    }

    void runTick() {
        forEachObject([](BaseObject &object, int ind) {
            object.onTick();
        });
    }

    void forEachInRadius(Vector2 pos, float radius, std::function<void(BaseObject*, int)> callback) {
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

};