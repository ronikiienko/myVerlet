#pragma once

#include <stdexcept>
#include <vector>
#include <array>
#include "../utils/Rectangle.h"

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
//    template<typename Func>
//    void forEachId(Func &&m_callback) const {
//        for (int i = 0; i < activeCount; i++) {
//            m_callback(ids[i], i);
//        }
//    }
//
//    void clear() {
//        activeCount = 0;  // Reset counter. No need to actually clear the vector.
//    }
//};

// this cell  uses fixed size array - more performant. but it relies on fact that only 4 m_objects can fit in one cell (all should have same radius). Commented one - vector, which allows different radiuses.
struct Cell {
    std::array<int, 4> ids = {0,0,0,0};  // Fixed-size array
    char activeCount = 0;  // Keeps track of how many ids are currently active.

    void insert(int id) {
        if (activeCount < 4) {
            ids[activeCount] = id;
        } else {
            // TODO throw if cell overflow happens. For now i spawn many m_objects randomly and one can be in other, so overflow will initially happen
            // TODO overflow can happen also if many object are dense and "smashed" together. It happens pretty rare so probably can ignore that
//            throw std::runtime_error("cell overflow");
        }
        activeCount++;
    }

    // good thing but affects performance. Because should be called for each cell.
//    template<typename Func>
//    void forEachId(Func&& m_callback)const {
//        for (int i = 0; i < activeCount; i++) {
//            m_callback(ids[i], i);
//        }
//    }

    void clear() {
        activeCount = 0;  // Reset counter
    }
};

struct IdGrid {
    int m_width;
    int m_height;
    int m_realWidth;
    int m_realHeight;
    float m_widthRatio;
    float m_heightRatio;
    float m_inverseWidthRatio;
    float m_inverseHeightRatio;
    std::vector<Cell> m_data;
    int m_length;

    IdGrid(int width, int height, Vector2I realSize) : m_width(width), m_height(height) {
        m_realWidth = realSize.m_x;
        m_realHeight = realSize.m_y;

        m_widthRatio = static_cast<float>(m_realWidth) / static_cast<float>(width);
        m_heightRatio = static_cast<float>(m_realHeight) / static_cast<float>(height);
        m_inverseWidthRatio = 1 / m_widthRatio;
        m_inverseHeightRatio = 1 / m_heightRatio;

        m_length = width * height;
        m_data.resize(m_length);
    }

    void insert(int id, float realX, float realY) {
        // Convert real-m_scene coordinates to m_grid coordinates
        const int gridX = static_cast<int>(realX * m_inverseWidthRatio);
        const int gridY = static_cast<int>(realY * m_inverseHeightRatio);

        // TODO remove checks - they are for debuggin
        if (gridX < 0 || gridX >= m_width || gridY < 0 || gridY >= m_height) {
            throw std::runtime_error("Trying to set outside the m_grid. Grid m_x: " + std::to_string(gridX) + " Grid m_y: " + std::to_string(gridY) + " Id: " + std::to_string(id));
        }

        const int index = gridY * m_width + gridX;

        m_data[index].insert(id);
    }

    void clear() {
        for (auto &cell: m_data) {
            cell.clear();
        }
    }

    void clear(int startCellIndex, int endCellIndex) {
        for (int i = startCellIndex; i < endCellIndex; i++) {
            m_data[i].clear();
        }
    }

    [[nodiscard]] const Cell& get(int gridX, int gridY) const {
        // TODO remove safety checks
        if (gridX < 0 || gridX >= m_width || gridY < 0 || gridY >= m_height) {
            throw std::runtime_error("Trying to get outside the m_grid.");
        }
        int index = gridY * m_width + gridX;
        return m_data[index];
    }

    // it's not precise iterator. it just uses m_grid cells to iterate. So it's not precise, but it's fast. To actually iterate precise, needs more checks (it's just a broad phase)
    template<typename Func>
    void forEachInRect(RectangleF rect, Func &&callback) {
        const int startGridX = std::max(0, static_cast<int>(rect.getX1() * m_inverseWidthRatio));
        const int endGridX = std::min(m_width - 1, static_cast<int>(rect.getX2() * m_inverseWidthRatio));
        const int startGridY = std::max(0, static_cast<int>(rect.getY1() * m_inverseHeightRatio));
        const int endGridY = std::min(m_height - 1, static_cast<int>(rect.getY2() * m_inverseHeightRatio));

        for (int i = startGridX; i <= endGridX; i++) {
            for (int j = startGridY; j <= endGridY; j++) {
                const Cell &cell = get(i,j);

                for (int k = 0; k < cell.activeCount; k++) {
                    callback(cell.ids[k]);
                }
            }
        }
    }
};