#pragma once

#include <stdexcept>
#include <vector>
#include <array>
#include <functional>
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
    std::array<int, 4> ids = {0, 0, 0, 0};  // Fixed-size array
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
    float m_cellWidth;
    float m_cellHeight;
    float m_cellWidthInverse;
    float m_cellHeightInverse;
    std::vector<Cell> m_data;
    int m_length;

    IdGrid(int width, int height, Vector2I realSize) : m_width(width), m_height(height) {
        m_realWidth = realSize.m_x;
        m_realHeight = realSize.m_y;

        m_cellWidth = static_cast<float>(m_realWidth) / static_cast<float>(width);
        m_cellHeight = static_cast<float>(m_realHeight) / static_cast<float>(height);
        m_cellWidthInverse = 1 / m_cellWidth;
        m_cellHeightInverse = 1 / m_cellHeight;

        m_length = width * height;
        m_data.resize(m_length);
    }

    // this method is called huge number of times
    void insert(int id, float realX, float realY) {
        // Convert real-m_scene coordinates to m_grid coordinates
        const int gridX = static_cast<int>(realX * m_cellWidthInverse);
        const int gridY = static_cast<int>(realY * m_cellHeightInverse);

        // TODO remove checks - they are for debuggin
        if (gridX < 0 || gridX >= m_width || gridY < 0 || gridY >= m_height) {
            throw std::runtime_error(
                    "Trying to set outside the m_grid. Grid m_x: " + std::to_string(gridX) + " Grid m_y: " +
                    std::to_string(gridY) + " Id: " + std::to_string(id));
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

    [[nodiscard]] const Cell &get(int gridX, int gridY) const {
        // TODO remove safety checks
        if (gridX < 0 || gridX >= m_width || gridY < 0 || gridY >= m_height) {
            throw std::runtime_error("Trying to get outside the m_grid.");
        }
        int index = gridY * m_width + gridX;
        return m_data[index];
    }

    [[nodiscard]] int realXToGridX(float realX) const {
        return static_cast<int>(realX * m_cellWidthInverse);
    }

    [[nodiscard]] int realYToGridY(float realY) const {
        return static_cast<int>(realY * m_cellHeightInverse);
    }

    // it's not precise iterator. it just uses m_grid cells to iterate. So it's not precise, but it's fast. To actually iterate precise, needs more checks (it's just a broad phase)
    template<typename T>
    void forEachInRect(RectangleF rect, const T &callback) const {
        const int startGridX = std::max(0, realXToGridX(rect.getX1()));
        const int endGridX = std::min(m_width - 1, realXToGridX(rect.getX2()));
        const int startGridY = std::max(0, realYToGridY(rect.getY1()));
        const int endGridY = std::min(m_height - 1, realYToGridY(rect.getY2()));

        for (int i = startGridX; i <= endGridX; i++) {
            for (int j = startGridY; j <= endGridY; j++) {
                const Cell &cell = get(i, j);

                for (int k = 0; k < cell.activeCount; k++) {
                    callback(cell.ids[k]);
                }
            }
        }
    }

    void clipLinePoints(Vector2F &start, Vector2F &end, float gridPadding) const {
        float minX = 0 + gridPadding;
        float minY = 0 + gridPadding;
        float maxX = static_cast<float>(m_width) - gridPadding;
        float maxY = static_cast<float>(m_height) - gridPadding;

        Vector2F vectorToClippedStart = end - start;

        if (start.m_x < minX) {
            vectorToClippedStart.limitByXSaveRatio(minX - start.m_x);
        } else if (start.m_x > maxX) {
            vectorToClippedStart.limitByXSaveRatio(maxX - start.m_x);
        }
        if (start.m_y < minY) {
            vectorToClippedStart.limitByYSaveRatio(minY - start.m_y);
        } else if (start.m_y > maxY) {
            vectorToClippedStart.limitByYSaveRatio(maxY - start.m_y);
        }

        start += vectorToClippedStart;


        std::cout << "end was: " << end.m_x << " " << end.m_y << "\n";
        Vector2F vectorToClippedEnd = start - end;
        if (end.m_x < minX) {
            vectorToClippedEnd.limitByXSaveRatio(minX - end.m_x);
        } else if (end.m_x > maxX) {
            vectorToClippedEnd.limitByXSaveRatio(maxX - end.m_x);
        }

        if (end.m_y < minY) {
            vectorToClippedEnd.limitByYSaveRatio(minY - end.m_y);
        } else if (end.m_y > maxY) {
            vectorToClippedEnd.limitByYSaveRatio(maxY - end.m_y);
        }
        std::cout << "vectorToClippedEnd is: " << vectorToClippedEnd.m_x << " " << vectorToClippedEnd.m_y << "\n";
        end += vectorToClippedEnd;

        std::cout << "end is: " << end.m_x << " " << end.m_y << "\n";
    }

    // it's not precise. it's just a broad phase.
    // Iterates on every id in cells around line.
    // uses DDA algorithm, but with vectors.
    // this iterator is conservative (it can iterate more than needed, but not less)
    // to achieve it, it uses 1 cell around all cells that line passes through
    // it was important to avoid duplicate iterations.
    // To avoid them, it uses prevGridX and prevGridY
    template<typename T>
    void forEachAroundLine(Vector2F start, Vector2F end, const T &callback) const {
        Vector2F startGrid = start * m_cellWidthInverse;
        Vector2F endGrid = end * m_cellHeightInverse;
//        clipLinePoints(startGrid, endGrid, 1.1f);

        Vector2F deltaGrid = endGrid - startGrid;

        Vector2I cellsDelta = Vector2I::cart(static_cast<int>(endGrid.m_x) - static_cast<int>(startGrid.m_x), static_cast<int>(endGrid.m_y) - static_cast<int>(startGrid.m_y));
        Vector2I cellsDistance = Vector2I::cart(std::abs(cellsDelta.m_x), std::abs(cellsDelta.m_y));

        int steps;

        if (cellsDistance.m_x > cellsDistance.m_y) {
            steps = cellsDistance.m_x;
        } else {
            steps = cellsDistance.m_y;
        }

        Vector2F increment = Vector2F::cart();
        if (steps != 0) {
            increment = deltaGrid / static_cast<float>(steps);
        }

        Vector2F current = startGrid;

        int prevGridX = -1000;
        int prevGridY = -1000;

        for (int i = 0; i <= steps; i++) {
            int gridX = static_cast<int>(current.m_x);
            int gridY = static_cast<int>(current.m_y);


            for (int column = gridX - 1; column <= gridX + 1; column++) {
                for (int row = gridY - 1; row <= gridY + 1; row++) {
                    if (
                            column >= prevGridX - 1 && column <= prevGridX + 1 &&
                            row >= prevGridY - 1 && row <= prevGridY + 1
                            ) {
                        continue;
                    }
                    const Cell &cell = get(column, row);
                    for (int k = 0; k < cell.activeCount; k++) {
                        callback(cell.ids[k]);
                    }
                }
            }


            current += increment;
            prevGridX = gridX;
            prevGridY = gridY;
        }
    }
};