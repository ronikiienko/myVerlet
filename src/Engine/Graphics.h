#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include "Scene/Scene.h"
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
    sf::VertexArray m_sticksVertexArray;
    ThreadPool &m_threadPool;
    sf::Texture m_objectTexture;
    PerformanceMonitor &m_performanceMonitor;
    float m_textureSize;

    sf::RectangleShape m_backgroundShape;
    sf::RectangleShape m_wallShape;
public:
    Graphics(Scene &scene, sf::RenderWindow &window, ThreadPool &threadPool,
             PerformanceMonitor &performanceMonitor)
            : m_scene(scene), m_window(window), m_threadPool(threadPool), m_performanceMonitor(performanceMonitor) {
        m_objectVertexArray.setPrimitiveType(sf::Quads);
        m_rotationsVertexArray.setPrimitiveType(sf::Quads);
        m_sticksVertexArray.setPrimitiveType(sf::Quads);
        // TODO somehow organise resources, because now path depends on where executable is. Same for fonts in performance monitor
        if (!m_objectTexture.loadFromFile("./res/circle.png")) {
            throw std::runtime_error("Could not load circle texture file");
        }
        m_objectTexture.generateMipmap();
        m_objectTexture.setSmooth(true);
        m_textureSize = 1024.0f;
    };

    void updateObjectsArray() {
        m_objectVertexArray.resize(m_scene.getObjectStorage().getObjectsCount() * 4);
        m_threadPool.dispatch(m_scene.getObjectStorage().getObjectsCount(), [this](int start, int end) {
            float objectSize = engineDefaults::objectsRadius;
            m_scene.getObjectStorage().forEachBasicDetails([this, objectSize](BasicDetails &object, int index) {
                Vector2F screenPos = object.m_posCurr;

                const int ind = index * 4;

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
        m_rotationsVertexArray.resize(m_scene.getObjectStorage().getObjectsWithRotationCount() * 4);
        m_threadPool.dispatch(m_scene.getObjectStorage().getObjectsWithRotationCount(), [this](int start, int end) {
            float objectSize = engineDefaults::rotationCircleRadius;
            m_scene.getObjectStorage().forEachBasicDetailsWithRotation([this, objectSize](BasicDetails &object, int index, int iteration) {
                Vector2F screenPos = object.m_posCurr + object.m_direction;

                const int ind = iteration * 4;

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

    void updateSticksArray() {
        m_sticksVertexArray.resize(m_scene.getStickStorage().getSticksCount() * 4);
        m_threadPool.dispatch(m_scene.getStickStorage().getSticksCount(), [this](int start, int end) {
            m_scene.getStickStorage().forEachBasicStickDetails([this](BasicStickDetails &stick, int index) {
                Vector2F pos1 = m_scene.getObjectStorage().getBasicDetails(stick.m_id1).m_posCurr;
                Vector2F pos2 = m_scene.getObjectStorage().getBasicDetails(stick.m_id2).m_posCurr;

                Vector2F diff = pos2 - pos1;
                Vector2F perp = diff.perpClockwise().normalize();


                const int ind = index * 4;

                Vector2F p1 = pos1 + perp * engineDefaults::stickThickness / 2;
                Vector2F p2 = pos2 + perp * engineDefaults::stickThickness / 2;
                Vector2F p3 = pos2 - perp * engineDefaults::stickThickness / 2;
                Vector2F p4 = pos1 - perp * engineDefaults::stickThickness / 2;


                m_sticksVertexArray[ind].position = {p1.m_x, p1.m_y};
                m_sticksVertexArray[ind + 1].position = {p2.m_x, p2.m_y};
                m_sticksVertexArray[ind + 2].position = {p3.m_x, p3.m_y};
                m_sticksVertexArray[ind + 3].position = {p4.m_x, p4.m_y};

                m_sticksVertexArray[ind].color = stick.m_color;
                m_sticksVertexArray[ind + 1].color = stick.m_color;
                m_sticksVertexArray[ind + 2].color = stick.m_color;
                m_sticksVertexArray[ind + 3].color = stick.m_color;
            }, start, end);
        });
    }

    void updateWalls() {
        float wallsThickness = 40;

        Vector2F startPos = Vector2F::cart(-wallsThickness, -wallsThickness);
        Vector2F size = Vector2F::cart(
                m_scene.getSizeF().m_x + wallsThickness * 2,
                m_scene.getSizeF().m_y + wallsThickness * 2
        );

        m_wallShape.setFillColor(sf::Color{255, 255, 255, 30});
        m_wallShape.setPosition(sf::Vector2f(startPos.m_x, startPos.m_y));
        m_wallShape.setSize(sf::Vector2f(size.m_x, size.m_y));

        m_window.draw(m_wallShape);
    }

    void updateBackground() {
        Vector2F startPos = Vector2F::cart(0, 0);
        Vector2F size = Vector2F::cart(
                m_scene.getSizeF().m_x,
                m_scene.getSizeF().m_y
        );

        m_backgroundShape.setFillColor(sf::Color::Black);
        m_backgroundShape.setPosition(sf::Vector2f(startPos.m_x, startPos.m_y));
        m_backgroundShape.setSize(sf::Vector2f(size.m_x, size.m_y));

        m_window.draw(m_backgroundShape);
    }

    void update() {
        m_performanceMonitor.start("walls");
        updateWalls();
        updateBackground();
        m_performanceMonitor.end("walls");
        m_performanceMonitor.start("objects");
        updateObjectsArray();
        m_performanceMonitor.end("objects");
        m_performanceMonitor.start("rotations");
        updateRotationsArray();
        m_performanceMonitor.end("rotations");
        m_performanceMonitor.start("sticks");
        updateSticksArray();
        m_performanceMonitor.end("sticks");
        m_performanceMonitor.start("draw");
        m_window.draw(m_sticksVertexArray);
        m_window.draw(m_objectVertexArray, &m_objectTexture);
        m_window.draw(m_rotationsVertexArray, &m_objectTexture);
        m_performanceMonitor.end("draw");
    }

    Graphics(const Graphics &) = delete;

    Graphics &operator=(const Graphics &) = delete;

    Graphics(Graphics &&) = delete;

    Graphics &operator=(Graphics &&) = delete;
};
