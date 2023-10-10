#pragma once

#include <stdexcept>
#include <vector>
#include "Rectangle.h"

template<typename T>
struct Cell {
    std::vector<T> ids;

    void insert(T id) {
        ids.push_back(id);
    }

    void clear() {
        ids.clear();
    }
};

template<typename idType>
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
    std::vector<Cell<idType>> data;

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

    void insert(idType id, int realX, int realY) {
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
        for (Cell<idType> &cell: data) {
            cell.clear();
        }
    }
};