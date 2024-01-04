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
    Shooter shooter{Vector2::fromCartesian(200, 200), Angle::fromDegrees(45), 4, 1, 4, atomWorld, 1};
    RNGf gen{consts::seed};
    RandomSpawner randomSpawner{atomWorld, gen};
    Benchmark benchmark{60 * 30};
    Camera camera{static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y), Vector2::fromCartesian(0, 0)};
    PerformanceMonitor performanceMonitor{window, atomWorld};
    bool isRunning = true;

    Game() {
        window.setFramerateLimit(60);
        randomSpawner.spawn(150000);
        startLoop();
    };
    void startLoop() {
        while (window.isOpen()) {

            sf::Event event{};
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) {
                    camera.move(Vector2::fromCartesian(0, -40));
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                    camera.move(Vector2::fromCartesian(0, 40));
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                    camera.move(Vector2::fromCartesian(-40, 0));
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                    camera.move(Vector2::fromCartesian(40, 0));
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    shooter.pointTo(
                            camera.screenPosToWorldPos(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                                              static_cast<float>(event.mouseButton.y))));
                    shooter.shoot();
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                    explosionHandler.launch(
                            camera.screenPosToWorldPos(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                                              static_cast<float>(event.mouseButton.y))),
                            4,
                            150);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace) {
                    atomWorld.clear();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
                    randomSpawner.spawn(5000);
                }

                if (event.type == sf::Event::MouseWheelScrolled) {
                    // need to pass mouse position
                    Vector2 screenMousePos = Vector2::fromCartesian(static_cast<float>(sf::Mouse::getPosition(window).x),
                                                                    static_cast<float>(sf::Mouse::getPosition(window).y));
                    if (event.mouseWheelScroll.delta > 0) {
                        camera.zoom(1.5);
                    } else {
                        camera.zoom(0.75);
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    if (isRunning) {
                        stop();
                    } else {
                        run();
                    }
                }
            }

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
