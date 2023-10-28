#pragma once

#include <vector>
#include "VerletObject.h"
#include "../modules/Rectangle.h"
#include "VerletStick.h"

class World {
private:
    std::vector<VerletObject> objects;
    std::vector<VerletStick> sticks;
    RectangleF boundsF;
    RectangleI boundsI;
public:
    explicit World(RectangleI bounds) : boundsF(RectangleF::fromOther(bounds)), boundsI(bounds) {
        objects.reserve(maxObjectNum);
        sticks.reserve(maxSticksNum);
    }
    VerletObject &addObject(Vector2 position, float radius) {
        return objects.emplace_back(position, radius);
    }
    VerletStick &addStick(VerletObject& obj1, VerletObject& obj2) {
        return sticks.emplace_back(obj1, obj2);
    }

    [[nodiscard]] std::vector<VerletObject> &getObjects() {
        return objects;
    }

    [[nodiscard]] std::vector<VerletStick> &getSticks() {
        return sticks;
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