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
#include "addons/ExplosionHandler.h"
#include "addons/Shooter.h"
#include "addons/RandomSpawner.h"
#include "addons/Chain.h"
#include "addons/Benchmark.h"

bool isSpacePressed = false;

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
    Shooter shooter{Vector2::fromCartesian(200, 200), Angle::fromDegrees(45), 4, 1, 4, world, 6};
    RNGf gen = RNGf(seed);
    RandomSpawner randomSpawner{world, gen};
    Benchmark benchmark{60 * 30};
    randomSpawner.spawn(150000);


    VerletObject *objectHold = nullptr;
    bool wasPinned;
    while (window.isOpen()) {

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                const Vector2 clickPoint = Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                                  static_cast<float>(event.mouseButton.y));
                world.forEachObject([&](VerletObject &object, int i) {
                    if ((clickPoint - object.posCurr).magnitude() < objectsRadius) {
                        objectHold = &object;
                        wasPinned = object.isPinned;
                        object.isPinned = true;
                    }
                });
            }
            if (event.type == sf::Event::MouseMoved && objectHold != nullptr) {
                // Mouse was moved and there's an object being held
                objectHold->posCurr = Vector2::fromCartesian(static_cast<float>(event.mouseMove.x),
                                                             static_cast<float>(event.mouseMove.y));
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left &&
                objectHold != nullptr) {
                objectHold->isPinned = wasPinned;
                objectHold->posOld = objectHold->posCurr;
                objectHold = nullptr;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                isSpacePressed = true;
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
                isSpacePressed = false;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                shooter.rotate(Angle::fromDegrees(5));
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
                shooter.rotate(Angle::fromDegrees(-5));
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                shooter.move(Vector2::fromCartesian(0, -40));
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                shooter.move(Vector2::fromCartesian(0, 40));
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                shooter.move(Vector2::fromCartesian(-40, 0));
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                shooter.move(Vector2::fromCartesian(40, 0));
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                explosionHandler.launch(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                               static_cast<float>(event.mouseButton.y)), 4, 150);
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace) {
                world.clear();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
                randomSpawner.spawn(5000);
            }
        }
        if (isSpacePressed) {
            shooter.shoot();
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

        benchmark.sample();
    }


    return 0;
}
