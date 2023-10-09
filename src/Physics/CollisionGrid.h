#pragma once

#include "../modules/Grid.h"

struct CollisionCell {
    static constexpr short cellCapacity = 4;
    static constexpr short maxCellIndex = cellCapacity - 1;

    short objectsCount = 0;
    int objects[cellCapacity] = {};

    void addObject(int objectId) {
        objects[objectsCount] = objectId;
        if (objectsCount < maxCellIndex) {
            objectsCount += 1;
        }
    };

    void clear() {
        objectsCount = 0;
    }
};

struct CollisionGrid : public Grid<CollisionCell> {
    CollisionGrid(int width, int height) : Grid<CollisionCell>(width, height) {};

    void addObject(int cellX, int cellY, int objectId) {
        const int cellIndex = cellX * height + cellY;
        std::cout << cellIndex;
        cells[cellIndex].addObject(objectId);
    }

    void clear() {
        for (CollisionCell cell: cells) {
            cell.clear();
        }
    }
};