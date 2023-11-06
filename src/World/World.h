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
        auto complexObjectPtr = std::make_unique<T>(complexObject);
        complexObjects.push_back(std::move(complexObjectPtr));

        return getComplexObjectsCount() - 1;
    }


    [[nodiscard]] int getComplexObjectsCount() {
        return static_cast<int>(complexObjects.size());
    }
};