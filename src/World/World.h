#pragma once

#include <vector>
#include "ComplexObject.h"

class World {
private:
    AtomWorld& atomWorld;
    std::vector<std::unique_ptr<ComplexObject>> complexObjects;

public:
    explicit World(AtomWorld& atomWorld) : atomWorld(atomWorld) {};

    template<typename T>
    int addComplexObject(T &&complexObject) {
        complexObjects.push_back(std::make_unique<T>(std::forward<T>(complexObject)));
        return getComplexObjectsCount() - 1;
    }

    std::unique_ptr<ComplexObject>& getObject(int index) {
        return complexObjects[index];
    }

    [[nodiscard]] int getComplexObjectsCount() {
        return static_cast<int>(complexObjects.size());
    }
};