#pragma once

#include "World.h"
#include "SFML/Graphics/RenderWindow.hpp"

class Graphics {
private:
    World &world;
    sf::RenderWindow &window;
public:
    explicit Graphics(World &world, sf::RenderWindow &window) : world(world), window(window) {};

    void update() {
        window.clear(sf::Color::Black);
        sf::CircleShape circle{1.0f};
        circle.setPointCount(32);
        circle.setOrigin(1.0f, 1.0f);
        const std::vector<VerletObject> &objects = world.getObjects();
        for (const VerletObject &object: objects) {
            circle.setPosition(object.posCurr.x, object.posCurr.y);
            circle.setScale(object.radius, object.radius);
            circle.setFillColor(object.color);
            window.draw(circle);
        }
        window.display();
    }
};