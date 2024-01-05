#pragma once

#include "SFML/Graphics.hpp"
#include "../utils/Vector.h"
#include "../consts.h"
#include "../AtomWorld/AtomWorld.h"
#include "../Graphics/Graphics.h"
#include "../Physics/Physics.h"
#include "../utils/Rand.h"
#include "../utils/ThreadsTest.h"
#include "../utils/ThreadPool.h"
#include "../PerformanceMonitor/PerformanceMonitor.h"
#include "../addons/ExplosionHandler.h"
#include "../addons/Shooter.h"
#include "../addons/RandomSpawner.h"
#include "../addons/Benchmark.h"
#include "../Camera/Camera.h"
#include "../InputHandler/InputHandler.h"
#include "../AtomWorld/Player.h"

class Game {
public:
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(consts::windowBounds.getWidth(),
                                                             consts::windowBounds.getHeight()), "Verlet",
                                               sf::Style::Default, sf::ContextSettings(0, 0, 1));
    ThreadPool threadPool{consts::numThreads};
    AtomWorld atomWorld{consts::worldBounds};
    Graphics graphics{atomWorld, window, threadPool, performanceMonitor, camera};
    Physics physics{atomWorld, threadPool, performanceMonitor};
    ExplosionHandler explosionHandler{atomWorld};
    Shooter shooter{Vector2::fromCartesian(200, 200), 2, 1, 4, atomWorld, 1, explosionHandler};
    RNGf gen{consts::seed};
    RandomSpawner randomSpawner{atomWorld, gen};
    Benchmark benchmark{60 * 30};
    Camera camera{static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y),
                  Vector2::fromCartesian(0, 0)};
    PerformanceMonitor performanceMonitor{window, atomWorld};
    InputHandler inputHandler{window};
    bool isRunning = true;

    Game() {
        window.setFramerateLimit(60);
        randomSpawner.spawn(10000);
        int id = atomWorld.addObject(Player{Vector2::fromCartesian(100,100), inputHandler, camera, shooter});
        inputHandler.addEventListener(sf::Event::MouseWheelScrolled, [&](sf::Event &event) {
            if (event.mouseWheelScroll.delta > 0) {
                camera.zoom(1.5);
            } else {
                camera.zoom(0.75);
            }
        });

        inputHandler.addEventListener(sf::Event::KeyPressed, [&](sf::Event &event) {
            if (event.key.code == sf::Keyboard::BackSpace) {
                atomWorld.clear();
            }

            if (event.key.code == sf::Keyboard::M) {
                randomSpawner.spawn(5000);
            }
            if (event.key.code == sf::Keyboard::Escape) {
                if (isRunning) {
                    stop();
                } else {
                    run();
                }
            }
        });

        inputHandler.addEventListener(sf::Event::MouseButtonPressed, [&](sf::Event &event) {
//            if (event.mouseButton.button == sf::Mouse::Left) {
//                shooter.pointTo(
//                        camera.screenPosToWorldPos(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
//                                                                          static_cast<float>(event.mouseButton.y))));
//                shooter.shoot();
//            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                explosionHandler.launch(
                        camera.screenPosToWorldPos(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                                          static_cast<float>(event.mouseButton.y))),
                        4,
                        150);
            }
        });
        startLoop();
    };

    void startLoop() {
        while (window.isOpen()) {
            performanceMonitor.start("total");

            if (isRunning) {
                performanceMonitor.start("physics");
                physics.update();
                performanceMonitor.end("physics");
            }

            performanceMonitor.start("graphics");
            window.clear(sf::Color::Black);
            graphics.update();
            performanceMonitor.draw();
            performanceMonitor.end("graphics");
            window.display();

            performanceMonitor.start("input");
            inputHandler.update();
            performanceMonitor.end("input");

            performanceMonitor.start("onTick");
            atomWorld.runTick();
            performanceMonitor.end("onTick");

            performanceMonitor.end("total");

            benchmark.sample();
        }
    }

    void run() {
        isRunning = true;
    }

    void stop() {
        isRunning = false;
    }
};
