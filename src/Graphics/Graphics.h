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
//            circle.setScale(object.radius, object.radius);
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
    sf::VertexArray vertexArray;  // Added this line
    ThreadPool &threadPool;
    sf::Texture objectTexture;
    float textureSize;
public:
    explicit Graphics(World &world, sf::RenderWindow &window, ThreadPool &threadPool)
            : world(world), window(window), threadPool(threadPool) {
        vertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads

        if (!objectTexture.loadFromFile("../res/circle.png")) {
            throw std::runtime_error("Could not load circle texture file");
        }
        objectTexture.generateMipmap();
        objectTexture.setSmooth(true);
        textureSize = 1024.0f;
    };

    void update() {
        const std::vector<VerletObject> &objects = world.getObjects();
        const int objectsCount = world.getObjectsCount();

        vertexArray.resize(world.getObjectsCount() * 4);

        threadPool.dispatch(objectsCount, [this, &objects](int start, int end) {
            for (int i = start; i < end; i++) {
                const VerletObject &object = objects[i];

                const int ind = i * 4;

                vertexArray[ind].position = {object.posCurr.x - object.radius, object.posCurr.y - object.radius};
                vertexArray[ind+1].position = {object.posCurr.x + object.radius, object.posCurr.y - object.radius};
                vertexArray[ind+2].position = {object.posCurr.x + object.radius, object.posCurr.y + object.radius};
                vertexArray[ind+3].position = {object.posCurr.x - object.radius, object.posCurr.y + object.radius};

                vertexArray[ind].texCoords = {0.0f, 0.0f};
                vertexArray[ind+1].texCoords = {textureSize, 0.0f};
                vertexArray[ind+2].texCoords = {textureSize, textureSize};
                vertexArray[ind+3].texCoords = {0.0f, textureSize};

                vertexArray[ind].color = object.color;
                vertexArray[ind + 1].color = object.color;
                vertexArray[ind + 2].color = object.color;
                vertexArray[ind + 3].color = object.color;
            };
        });

        window.draw(vertexArray, &objectTexture);
    }
};
