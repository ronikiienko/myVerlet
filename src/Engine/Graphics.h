#pragma once

#include "Scene.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "PerformanceMonitor.h"
#include "Camera.h"
#include "utils/ThreadPool.h"
#include "EngineConsts.h"

class Graphics {
private:
    Scene &m_scene;
    sf::RenderWindow &m_window;
    sf::VertexArray m_objectVertexArray;
    sf::VertexArray m_rotationsVertexArray;
    ThreadPool &m_threadPool;
    sf::Texture m_objectTexture;
    PerformanceMonitor &m_performanceMonitor;
    float m_textureSize;
public:
    Graphics(Scene &scene, sf::RenderWindow &window, ThreadPool &threadPool,
                      PerformanceMonitor &performanceMonitor)
            : m_scene(scene), m_window(window), m_threadPool(threadPool), m_performanceMonitor(performanceMonitor) {
        m_objectVertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads
        m_rotationsVertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads
        // TODO somehow organise resources, because now path depends on where executable is. Same for fonts in performance monitor
        if (!m_objectTexture.loadFromFile("./res/circle.png")) {
            throw std::runtime_error("Could not load circle texture file");
        }
        m_objectTexture.generateMipmap();
        m_objectTexture.setSmooth(true);
        m_textureSize = 1024.0f;
    };

    void updateObjectsArray() {
        m_objectVertexArray.resize(m_scene.getObjectsCount() * 4);
        m_threadPool.dispatch(m_scene.getObjectsCount(), [this](int start, int end) {
            float objectSize = m_scene.getCamera().worldScalarToScreen(engineDefaults::objectsRadius);
            m_scene.forEachBasicDetails([this, objectSize](BasicDetails &object, int i) {
                Vector2F screenPos = m_scene.getCamera().worldPosToScreenPos(object.m_posCurr);

                const int ind = i * 4;

                m_objectVertexArray[ind].position = {screenPos.m_x - objectSize, screenPos.m_y - objectSize};
                m_objectVertexArray[ind + 1].position = {screenPos.m_x + objectSize,
                                                       screenPos.m_y - objectSize};
                m_objectVertexArray[ind + 2].position = {screenPos.m_x + objectSize,
                                                       screenPos.m_y + objectSize};
                m_objectVertexArray[ind + 3].position = {screenPos.m_x - objectSize,
                                                       screenPos.m_y + objectSize};

                m_objectVertexArray[ind].texCoords = {0.0f, 0.0f};
                m_objectVertexArray[ind + 1].texCoords = {m_textureSize, 0.0f};
                m_objectVertexArray[ind + 2].texCoords = {m_textureSize, m_textureSize};
                m_objectVertexArray[ind + 3].texCoords = {0.0f, m_textureSize};

                m_objectVertexArray[ind].color = object.m_color;
                m_objectVertexArray[ind + 1].color = object.m_color;
                m_objectVertexArray[ind + 2].color = object.m_color;
                m_objectVertexArray[ind + 3].color = object.m_color;
            }, start, end);
        });
    }

    void updateRotationsArray() {
        m_rotationsVertexArray.resize(m_scene.getObjectsCount() * 4 / 16);
        m_threadPool.dispatch(m_scene.getObjectsCount() / 16, [this](int start, int end) {
            float objectSize = m_scene.getCamera().worldScalarToScreen(0.5);
            m_scene.forEachBasicDetails([this, objectSize](BasicDetails &object, int i) {
                Vector2F screenPos = m_scene.getCamera().worldPosToScreenPos(object.m_posCurr + object.m_direction);

                const int ind = i * 4;

                m_rotationsVertexArray[ind].position = {screenPos.m_x - objectSize, screenPos.m_y - objectSize};
                m_rotationsVertexArray[ind + 1].position = {screenPos.m_x + objectSize,
                                                         screenPos.m_y - objectSize};
                m_rotationsVertexArray[ind + 2].position = {screenPos.m_x + objectSize,
                                                         screenPos.m_y + objectSize};
                m_rotationsVertexArray[ind + 3].position = {screenPos.m_x - objectSize,
                                                         screenPos.m_y + objectSize};

                m_rotationsVertexArray[ind].texCoords = {0.0f, 0.0f};
                m_rotationsVertexArray[ind + 1].texCoords = {m_textureSize, 0.0f};
                m_rotationsVertexArray[ind + 2].texCoords = {m_textureSize, m_textureSize};
                m_rotationsVertexArray[ind + 3].texCoords = {0.0f, m_textureSize};

                m_rotationsVertexArray[ind].color = sf::Color::Black;
                m_rotationsVertexArray[ind + 1].color = sf::Color::Black;
                m_rotationsVertexArray[ind + 2].color = sf::Color::Black;
                m_rotationsVertexArray[ind + 3].color = sf::Color::Black;
            }, start, end);
        });
    }

    void update() {
        updateObjectsArray();
//        updateRotationsArray();
        m_window.draw(m_objectVertexArray, &m_objectTexture);
//        m_window.draw(m_rotationsVertexArray, &m_objectTexture);
    }

    Graphics(const Graphics &) = delete;
    Graphics& operator=(const Graphics &) = delete;
    Graphics(Graphics &&) = delete;
    Graphics& operator=(Graphics &&) = delete;
};
