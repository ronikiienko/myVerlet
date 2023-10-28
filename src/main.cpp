#include <iostream>
#include "SFML/Graphics.hpp"
#include <chrono>
#include "modules/Vector.h"
#include "consts.h"
#include "World/World.h"
#include "Graphics/Graphics.h"
#include "Physics/Physics.h"
#include "modules/Rand.h"
#include "modules/ThreadsTest.h"
#include "modules/ThreadPool.h"
#include "PerformanceMonitor/PerformanceMonitor.h"
#include "World/ExplosionHandler.h"

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowBounds.getWidth(), windowBounds.getHeight()), "Verlet",
                            sf::Style::Default, settings);
    window.setFramerateLimit(60);

    ThreadPool threadPool{numThreads};
    World world{worldBounds};
    PerformanceMonitor performanceMonitor = PerformanceMonitor{window, world};
    Graphics graphics{world, window, threadPool};
    Physics physics{world, threadPool, performanceMonitor};
    ExplosionHandler explosionHandler{world};

    RNGf gen = RNGf(seed);

    for (int i = 0; i < maxObjectNum; i++) {
        VerletObject &object = world.addObject(
                Vector2::fromCartesian(gen.getInRange(0, worldBounds.getWidth()),
                                       gen.getInRange(0, worldBounds.getHeight())),
                gen.getInRange(minRadius, maxRadius)
        );
        object.color = sf::Color(static_cast<int>(object.posCurr.x / worldBounds.getWidth() * 255),
                                 static_cast<int>(object.posCurr.y / worldBounds.getHeight() * 255), 255);
    }


    VerletObject* objectHold = nullptr;
    bool wasPinned;
    while (window.isOpen()) {

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                const Vector2 clickPoint = Vector2::fromCartesian(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                for (VerletObject& object : world.getObjects()) {
                    if ((clickPoint - object.posCurr).magnitude() < object.radius) {
                        objectHold = &object;
                        wasPinned = object.isPinned;
                        object.isPinned = true;
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved && objectHold != nullptr) {
                // Mouse was moved and there's an object being held
                objectHold->posCurr = Vector2::fromCartesian(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && objectHold != nullptr) {
                objectHold->isPinned = wasPinned;
                objectHold->posOld = objectHold->posCurr;
                objectHold = nullptr;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                for (int i = 0; i < 300; i++) {
                    const int centerX = worldBounds.getX2() / 2;
                    const int centerY = worldBounds.getY2() / 2;
                    VerletObject &newObject = world.addObject(
                            Vector2::fromCartesian(
                                    centerX + gen.getInRange(-120, 120),
                                    centerY + gen.getInRange(-60, 60)
                            ),
                            gen.getInRange(minRadius, maxRadius)
                    );
                    float velocityX = gen.getInRange(-1, 1);
                    float velocityY = gen.getInRange(-1,0);
                    newObject.setVelocity(Vector2::fromCartesian(velocityX, velocityY));
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                explosionHandler.launch(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x) , static_cast<float>(event.mouseButton.y)), 4, 500);
            }
        }

        performanceMonitor.start("total");

        performanceMonitor.start("physics");
        physics.update();
        performanceMonitor.end("physics");

        performanceMonitor.start("graphics");
        window.clear(sf::Color::Black);
        graphics.update();
        performanceMonitor.draw();
        performanceMonitor.end("graphics");
        window.display();

        performanceMonitor.end("total");
    }


    return 0;
}
