#pragma once

#include <vector>
#include <bits/shared_ptr.h>
#include "BaseObject.h"
#include "BaseStick.h"
#include "EngineConsts.h"

class AtomWorld {
private:
    std::vector<std::unique_ptr<BaseObject>> objects;
    RectangleF boundsF;
    RectangleI boundsI;
public:
    explicit AtomWorld(RectangleI bounds) : boundsF(RectangleF::fromOther(bounds)), boundsI(bounds) {
        objects.reserve(consts::maxObjectNum);
    }

    template<typename T>
    int addObject(T&& object) {
        std::unique_ptr<T> ptr = std::make_unique<T>(std::forward<T>(object));
        objects.push_back(std::move(ptr));
        int index = static_cast<int>(objects.size()) - 1;
        objects[index]->onInit();
        return getObjectsCount() - 1;
    }

    template <typename Func>
    void forEachObject(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(objects.size());
        }

        for (int i = start; i < end; i++) {
            callback(*objects[i], i);
        }
    }

    BaseObject& getObject(int ind) {
        return *objects[ind];
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
};