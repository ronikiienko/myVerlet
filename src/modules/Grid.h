#pragma once

#include <stdexcept>
#include <vector>
#include "Rectangle.h"

//struct Cell {
//    // when looping, you can't just loop through ids because there are old id's too - you need to loop between 0 and activeCount
//    // this optimisation doesn't give very much boost, but gives..
//    std::vector<int> ids;
//    int activeCount = 0;  // Keeps track of how many ids are currently active.
//
//    void insert(int id) {
//        if (activeCount < ids.size()) {
//            ids[activeCount] = id;  // Overwrite existing space.
//        } else {
//            ids.push_back(id);  // Add new space if needed.
//        }
//        activeCount++;
//    }
//
//    void clear() {
//        activeCount = 0;  // Reset counter. No need to actually clear the vector.
//    }
//};

// this cell  uses fixed size array - more performant. but it relies on fact that only 4 objects can fit in one cell (all should have same radius). Commented one - vector, which allows different radiuses.
struct Cell {
    std::array<int, 4> ids = {0,0,0,0};  // Fixed-size array
    int activeCount = 0;  // Keeps track of how many ids are currently active.

    void insert(int id) {
        if (activeCount < ids.size()) {
            ids[activeCount] = id;
        } else {
            // Handle overflow, maybe throw an exception or ignore
        }
        activeCount++;
    }

    void clear() {
        activeCount = 0;  // Reset counter
    }
};

struct IdGrid {
    int width;
    int height;
    int realX1;
    int realY1;
    int realX2;
    int realY2;
    int realWidth;
    int realHeight;
    float widthRatio;
    float heightRatio;
    std::vector<Cell> data;

    IdGrid(int width, int height, RectangleI realBounds) : width(width), height(height) {
        realX1 = realBounds.getX1();
        realY1 = realBounds.getY1();
        realX2 = realBounds.getX2();
        realY2 = realBounds.getY2();
        realWidth = realBounds.getWidth();
        realHeight = realBounds.getHeight();

        widthRatio = static_cast<float>(realWidth) / static_cast<float>(width);
        heightRatio = static_cast<float>(realHeight) / static_cast<float>(height);

        data.resize(width * height);
    }

    void insert(int id, int realX, int realY) {
        // Convert real-world coordinates to grid coordinates
        int gridX = static_cast<int>((realX - realX1) / widthRatio);
        int gridY = static_cast<int>((realY - realY1) / heightRatio);

        // TODO remove checks - they are for debuggin
        if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height) {
            throw std::runtime_error("Trying to set outside the grid. Grid x: " + std::to_string(gridX) + " Grid y: " + std::to_string(gridY));;
        }

        int index = gridY * width + gridX;

        data[index].insert(id);
    }

    void clear() {
        for (auto &cell: data) {
            cell.clear();
        }
    }

    [[nodiscard]] const Cell& get(int gridX, int gridY) const {
        if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height) {
            throw std::runtime_error("Trying to get outside the grid.");
        }
        int index = gridY * width + gridX;
        return data[index];
    }
};