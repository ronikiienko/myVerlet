#pragma once

#include "Scene.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "PerformanceMonitor.h"
#include "Camera.h"
#include "utils/ThreadPool.h"

class Graphics {
private:
    Scene &scene;
    sf::RenderWindow &window;
    sf::VertexArray objectVertexArray;
    ThreadPool &threadPool;
    sf::Texture objectTexture;
    PerformanceMonitor &performanceMonitor;
    float textureSize;
public:
    explicit Graphics(Scene &scene, sf::RenderWindow &window, ThreadPool &threadPool,
                      PerformanceMonitor &performanceMonitor)
            : scene(scene), window(window), threadPool(threadPool), performanceMonitor(performanceMonitor) {
        objectVertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads
        // TODO somehow organise resources, because now path depends on where executable is. Same for fonts in performance monitor
        if (!objectTexture.loadFromFile("./res/circle.png")) {
            throw std::runtime_error("Could not load circle texture file");
        }
        objectTexture.generateMipmap();
        objectTexture.setSmooth(true);
        textureSize = 1024.0f;
    };

    void updateObjectsArray() {
        objectVertexArray.resize(scene.getObjectsCount() * 4);
        threadPool.dispatch(scene.getObjectsCount(), [this](int start, int end) {
            float objectSize = scene.getCamera().worldSizeToScreenSize(engineDefaults::objectsRadius);
            scene.forEachBasicDetails([this, objectSize](BasicDetails &object, int i) {
                Vector2F screenPos = scene.getCamera().worldPosToScreenPos(object.posCurr);

                const int ind = i * 4;

                objectVertexArray[ind].position = {screenPos.x - objectSize, screenPos.y - objectSize};
                objectVertexArray[ind + 1].position = {screenPos.x + objectSize,
                                                       screenPos.y - objectSize};
                objectVertexArray[ind + 2].position = {screenPos.x + objectSize,
                                                       screenPos.y + objectSize};
                objectVertexArray[ind + 3].position = {screenPos.x - objectSize,
                                                       screenPos.y + objectSize};

                objectVertexArray[ind].texCoords = {0.0f, 0.0f};
                objectVertexArray[ind + 1].texCoords = {textureSize, 0.0f};
                objectVertexArray[ind + 2].texCoords = {textureSize, textureSize};
                objectVertexArray[ind + 3].texCoords = {0.0f, textureSize};

                objectVertexArray[ind].color = object.color;
                objectVertexArray[ind + 1].color = object.color;
                objectVertexArray[ind + 2].color = object.color;
                objectVertexArray[ind + 3].color = object.color;
            }, start, end);
        });
    }

    void update() {
        updateObjectsArray();

        window.draw(objectVertexArray, &objectTexture);
    }
};
