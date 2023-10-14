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
        window.clear(sf::Color::Black);

        // Clear previous vertex data
        vertexArray.clear();

        const std::vector<VerletObject> &objects = world.getObjects();

        for (const VerletObject &object: objects) {
            // Compute the four corners of the square representing the circle
            sf::Vertex one, two, three, four;

            one.position = {object.posCurr.x - object.radius, object.posCurr.y};
            two.position = {object.posCurr.x, object.posCurr.y - object.radius};
            three.position = {object.posCurr.x + object.radius, object.posCurr.y};
            four.position = {object.posCurr.x, object.posCurr.y + object.radius};

            // Set the color for each vertex
            one.color = object.color;
            two.color = object.color;
            three.color = object.color;
            four.color = object.color;

            // Append the vertices to the vertex array
            vertexArray.append(one);
            vertexArray.append(two);
            vertexArray.append(three);
            vertexArray.append(four);
        }

        // Draw all objects in one go!
        window.draw(vertexArray);

        window.display();
    }
};
