#pragma once

#include <vector>
#include "../World/ComplexObject.h"

class World {
private:
    std::vector<ComplexObject> complexObjects;

public:
    int addComplexObject(ComplexObject &&complexObject) {
        complexObjects.push_back(std::move(complexObject));
        return getComplexObjectsCount() - 1;
    }

    [[nodiscard]] int getComplexObjectsCount() {
        return static_cast<int>(complexObjects.size());
    }
};