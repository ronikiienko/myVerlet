#pragma once

#include <vector>
#include "VerletObject.h"

class World {
private:
    std::vector<VerletObject> objects;
public:
    VerletObject &addObject(Vector2 position, float radius) {
        return objects.emplace_back(position, radius);
    }

    [[nodiscard]] std::vector<VerletObject> &getObjects() {
        return objects;
    }

    [[nodiscard]] int getObjectsCount() {
        return static_cast<int>(objects.size());
    }
};