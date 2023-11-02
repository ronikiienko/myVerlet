//#pragma once
//
//#include "../World/World.h"
//#include "SFML/Graphics/RenderWindow.hpp"
//
//class Graphics {
//private:
//    World &world;
//    sf::RenderWindow &window;
//public:
//    explicit Graphics(World &world, sf::RenderWindow &window) : world(world), window(window) {};
//
//    void update() {
//        window.clear(sf::Color::Black);
//        sf::CircleShape circle{1.0f};
//        circle.setPointCount(6);
//        circle.setOrigin(1.0f, 1.0f);
//        const std::vector<VerletObject> &objects = world.getObjects();
//        for (const VerletObject &object: objects) {
//            circle.setPosition(object.posCurr.x, object.posCurr.y);
//            circle.setScale(objectsRadius, object.radius);
//            circle.setFillColor(object.color);
//            window.draw(circle);
//        }
//        window.display();
//    }
//};

#pragma once

#include "../World/World.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/VertexArray.hpp"
#include "../modules/ThreadPool.h"

class Graphics {
private:
    World &world;
    sf::RenderWindow &window;
    sf::VertexArray objectVertexArray;
    sf::VertexArray sticksVertexArray;
    ThreadPool &threadPool;
    sf::Texture objectTexture;
    float textureSize;
public:
    explicit Graphics(World &world, sf::RenderWindow &window, ThreadPool &threadPool)
            : world(world), window(window), threadPool(threadPool) {
        objectVertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads
        sticksVertexArray.setPrimitiveType(sf::Lines);
        // TODO somehow organise resources, because now path depends on where executable is. Same for fonts in performance monitor
        if (!objectTexture.loadFromFile("../res/circle.png")) {
            throw std::runtime_error("Could not load circle texture file");
        }
        objectTexture.generateMipmap();
        objectTexture.setSmooth(true);
        textureSize = 1024.0f;
    };

    void update() {
        objectVertexArray.resize(world.getObjectsCount() * 4);
        sticksVertexArray.resize(world.getSticksCount() * 2);

        threadPool.dispatch(world.getObjectsCount(), [this](int start, int end) {
            world.forEachObject([this](VerletObject& object, int i){
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

        threadPool.dispatch(world.getSticksCount(), [this](int start, int end) {
            world.forEachStick([this](VerletStick& stick, int i){
                const int ind = i * 2;

                sticksVertexArray[ind].position = {stick.obj1.posCurr.x, stick.obj1.posCurr.y};
                sticksVertexArray[ind + 1].position = {stick.obj2.posCurr.x, stick.obj2.posCurr.y};

                objectVertexArray[ind].color = stick.obj1.color;
                objectVertexArray[ind + 1].color = stick.obj2.color;
            }, start, end);
        });

        window.draw(objectVertexArray, &objectTexture);
        window.draw(sticksVertexArray);
    }
};
