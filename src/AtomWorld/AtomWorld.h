#pragma once

#include <vector>
#include "VerletObject.h"
#include "../modules/Rectangle.h"
#include "VerletStick.h"
#include <functional>

class AtomWorld {
private:
    std::vector<VerletObject> objects;
    std::vector<VerletStick> sticks;
    RectangleF boundsF;
    RectangleI boundsI;
public:
    explicit AtomWorld(RectangleI bounds) : boundsF(RectangleF::fromOther(bounds)), boundsI(bounds) {
        objects.reserve(maxObjectNum);
        sticks.reserve(maxSticksNum);
    }

    int addObject(Vector2 position) {
        objects.emplace_back(position);
        return getObjectsCount() - 1;
    }

    template <typename Func>
    void forEachObject(Func &&callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(objects.size());
        }

        for (int i = start; i < end; i++) {
            callback(objects[i], i);
        }
    }

    VerletObject& getObject(int ind) {
        return objects[ind];
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
};