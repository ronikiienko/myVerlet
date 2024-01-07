#pragma once

#include "AtomWorld.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "PerformanceMonitor.h"
#include "Camera.h"
#include "utils/ThreadPool.h"

class Graphics {
private:
    AtomWorld &atomWorld;
    sf::RenderWindow &window;
    sf::VertexArray objectVertexArray;
    ThreadPool &threadPool;
    sf::Texture objectTexture;
    PerformanceMonitor &performanceMonitor;
    Camera &camera;
    float textureSize;
public:
    explicit Graphics(AtomWorld &atomWorld, sf::RenderWindow &window, ThreadPool &threadPool,
                      PerformanceMonitor &performanceMonitor, Camera &camera)
            : atomWorld(atomWorld), window(window), threadPool(threadPool), performanceMonitor(performanceMonitor),
              camera(camera) {
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
        objectVertexArray.resize(atomWorld.getObjectsCount() * 4);
        threadPool.dispatch(atomWorld.getObjectsCount(), [this](int start, int end) {
            float objectSize = camera.worldSizeToScreenSize(consts::objectsRadius);
            atomWorld.forEachBasicDetails([this, objectSize](BasicDetails &object, int i) {
                Vector2 screenPos = camera.worldPosToScreenPos(object.posCurr);

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
