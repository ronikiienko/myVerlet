#pragma once

#include "../modules/Grid.h"

struct CollisionCell {
    static constexpr short cellCapacity = 4;
    static constexpr short maxCellIndex = cellCapacity - 1;

    short objectsCount = 2;
    int objects[cellCapacity] = {};

    CollisionCell() = default;

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
        const int cellIndex = cellY * width + cellX;
        cells[cellIndex].addObject(objectId);
    }

    void clear() {
        for (CollisionCell& cell: cells) {
            cell.clear();
        }
    }
};