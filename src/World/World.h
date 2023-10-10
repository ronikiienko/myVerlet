#pragma once

#include <vector>
#include "VerletObject.h"
#include "../modules/Rectangle.h"

class World {
private:
    std::vector<VerletObject> objects;
    RectangleF boundsF;
    RectangleI boundsI;
public:
    explicit World(RectangleI bounds) : boundsF(RectangleF::fromOther(bounds)), boundsI(bounds) {}
    VerletObject &addObject(Vector2 position, float radius) {
        return objects.emplace_back(position, radius);
    }

    [[nodiscard]] std::vector<VerletObject> &getObjects() {
        return objects;
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
};