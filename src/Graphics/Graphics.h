#pragma once

#include "../AtomWorld/AtomWorld.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "../utils/ThreadPool.h"
#include "../PerformanceMonitor/PerformanceMonitor.h"

class Graphics {
private:
    AtomWorld &atomWorld;
    sf::RenderWindow &window;
    sf::VertexArray objectVertexArray;
    sf::VertexArray sticksVertexArray;
    ThreadPool &threadPool;
    sf::Texture objectTexture;
    PerformanceMonitor& performanceMonitor;
    float textureSize;
public:
    explicit Graphics(AtomWorld &atomWorld, sf::RenderWindow &window, ThreadPool &threadPool, PerformanceMonitor& performanceMonitor)
            : atomWorld(atomWorld), window(window), threadPool(threadPool), performanceMonitor(performanceMonitor) {
        objectVertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads
        sticksVertexArray.setPrimitiveType(sf::Lines);
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
            atomWorld.forEachObject([this](VerletObject& object, int i){
                const int ind = i * 4;

                objectVertexArray[ind].position = {object.posCurr.x - objectsRadius, object.posCurr.y - objectsRadius};
                objectVertexArray[ind + 1].position = {object.posCurr.x + objectsRadius, object.posCurr.y - objectsRadius};
                objectVertexArray[ind + 2].position = {object.posCurr.x + objectsRadius, object.posCurr.y + objectsRadius};
                objectVertexArray[ind + 3].position = {object.posCurr.x - objectsRadius, object.posCurr.y + objectsRadius};

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

    void updateSticksArray() {
        sticksVertexArray.resize(atomWorld.getSticksCount() * 2);
        threadPool.dispatch(atomWorld.getSticksCount(), [this](int start, int end) {
            atomWorld.forEachStick([this](VerletStick& stick, int i){
                const int ind = i * 2;

                sticksVertexArray[ind].position = {stick.obj1.posCurr.x, stick.obj1.posCurr.y};
                sticksVertexArray[ind + 1].position = {stick.obj2.posCurr.x, stick.obj2.posCurr.y};

                objectVertexArray[ind].color = stick.obj1.color;
                objectVertexArray[ind + 1].color = stick.obj2.color;
            }, start, end);
        });
    }

    void update() {
        updateObjectsArray();
        updateSticksArray();

        window.draw(objectVertexArray, &objectTexture);
        window.draw(sticksVertexArray);
    }
};
