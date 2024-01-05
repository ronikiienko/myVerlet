#pragma once

#include <vector>
#include "VerletObject.h"
#include "VerletStick.h"
#include "../utils/Rectangle.h"
#include "../consts.h"
#include "../utils/Vector.h"

class AtomWorld {
private:
    std::vector<std::unique_ptr<VerletObject>> objects;
    std::vector<VerletStick> sticks;
    RectangleF boundsF;
    RectangleI boundsI;
public:
    explicit AtomWorld(RectangleI bounds) : boundsF(RectangleF::fromOther(bounds)), boundsI(bounds) {
        objects.reserve(consts::maxObjectNum);
        sticks.reserve(consts::maxSticksNum);
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

    VerletObject& getObject(int ind) {
        return *objects[ind];
    }

    VerletStick &addStick(VerletObject& obj1, VerletObject& obj2) {
        return sticks.emplace_back(obj1, obj2);
    }
    int addStick(int ind1, int ind2) {
        sticks.emplace_back(getObject(ind1), getObject(ind2));
        return getSticksCount() - 1;
    }

    template <typename Func>
    void forEachStick(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(sticks.size());
        }

        for (int i = start; i < end; i++) {
            callback(sticks[i], i);
        }
    }

    VerletStick &getStick(int id) {
        return sticks[id];
    }

    void clear() {
        objects.clear();
        sticks.clear();
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(objects.size());
    }


    [[nodiscard]] int getSticksCount() {
        return static_cast<int>(sticks.size());
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
        forEachObject([](VerletObject &object, int ind) {
            object.onTick();
        });
    }
};