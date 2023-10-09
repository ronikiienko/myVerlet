#pragma once

#include <vector>
#include "VerletObject.h"
#include "../modules/Rectangle.h"

class World {
private:
    std::vector<VerletObject> objects;
    Rectangle bounds;
public:
    explicit World(Rectangle bounds) : bounds(bounds) {};
    VerletObject &addObject(Vector2 position, float radius) {
        return objects.emplace_back(position, radius);
    }

    [[nodiscard]] std::vector<VerletObject> &getObjects() {
        return objects;
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(objects.size());
    }

    [[nodiscard]] Rectangle &getBounds() {
        return bounds;
    }
};