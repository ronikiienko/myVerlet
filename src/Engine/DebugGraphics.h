#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include "Scene.h"

class DebugGraphics {
private:
    Scene &m_scene;
    sf::RenderWindow &m_window;
    ThreadPool &m_threadPool;
    PerformanceMonitor &m_performanceMonitor;
    sf::VertexArray m_linesVertexArray{sf::Lines};
    bool m_isGridOn = false;

    void updateGridVertexArray() {
        int gridWidth = m_scene.m_grid.m_width;
        int gridHeight = m_scene.m_grid.m_height;
        m_linesVertexArray.resize(gridWidth * gridHeight + 4);
        int vertexIndex = 0;
        for (int column = 0; column <= gridWidth; column++) {
            Vector2F start = Vector2F::cart(static_cast<float>(column) * m_scene.m_grid.m_cellWidth, 0);
            Vector2F end = Vector2F::cart(static_cast<float>(column) * m_scene.m_grid.m_cellWidth, static_cast<float>(m_scene.m_grid.m_realHeight)) ;
            m_linesVertexArray[vertexIndex].position = sf::Vector2f(start.m_x, start.m_y);
            m_linesVertexArray[vertexIndex + 1].position = sf::Vector2f(end.m_x, end.m_y);
            vertexIndex += 2;
        }

        for (int row = 0; row <= gridHeight; row++) {
            Vector2F start = Vector2F::cart(0, static_cast<float>(row) * m_scene.m_grid.m_cellHeight) ;
            Vector2F end = Vector2F::cart(static_cast<float>(m_scene.m_grid.m_realWidth), static_cast<float>(row) * m_scene.m_grid.m_cellHeight);
            m_linesVertexArray[vertexIndex].position = sf::Vector2f(start.m_x, start.m_y);
            m_linesVertexArray[vertexIndex + 1].position = sf::Vector2f(end.m_x, end.m_y);
            vertexIndex += 2;
        }
    }
public:
    DebugGraphics(Scene &scene, sf::RenderWindow &window, ThreadPool &threadPool,
                  PerformanceMonitor &performanceMonitor)
            : m_scene(scene), m_window(window), m_threadPool(threadPool), m_performanceMonitor(performanceMonitor) {

    };
    void setDebugGridEnabled(bool enabled) {
        m_isGridOn = enabled;
    }
    void update() {
        if (m_isGridOn) {
            updateGridVertexArray();
            m_window.draw(m_linesVertexArray);
        }
    }
};