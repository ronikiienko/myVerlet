#pragma once

#include <vector>

template<typename T>
struct Grid {
    int width, height;
    std::vector<T> cells;

    Grid(int width, int height) : width(width), height(height) {
        cells.resize(width * height);
    };

    T &get(int x, int y) {
        return cells[y * width + x];
    }
};