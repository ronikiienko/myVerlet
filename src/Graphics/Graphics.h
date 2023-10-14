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
public:
    explicit Graphics(World &world, sf::RenderWindow &window, ThreadPool &threadPool)
            : world(world), window(window), threadPool(threadPool) {
        vertexArray.setPrimitiveType(sf::Quads);  // Initialize with Quads
    };

    void update() {
        vertexArray.resize(world.getObjectsCount() * 4);
        window.clear(sf::Color::Black);

        const std::vector<VerletObject> &objects = world.getObjects();
        const int objectsCount = world.getObjectsCount();
        threadPool.dispatch(objectsCount, [this, &objects](int start, int end) {
            for (int i = start; i < end; i++) {
                const VerletObject &object = objects[i];

                const int ind = i << 2;

                vertexArray[ind].position = {object.posCurr.x - object.radius, object.posCurr.y};
                vertexArray[ind+1].position = {object.posCurr.x, object.posCurr.y - object.radius};
                vertexArray[ind+2].position = {object.posCurr.x + object.radius, object.posCurr.y};
                vertexArray[ind+3].position = {object.posCurr.x, object.posCurr.y + object.radius};

                // Set the color for each vertex
                vertexArray[ind].color = object.color;
                vertexArray[ind + 1].color = object.color;
                vertexArray[ind + 2].color = object.color;
                vertexArray[ind + 3].color = object.color;
            };
        });

        // Draw all objects in one go!
        window.draw(vertexArray);

        window.display();
    }
};
