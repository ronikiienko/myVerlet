#include <iostream>
#include "SFML/Graphics.hpp"
#include <chrono>
#include "utils/Vector.h"
#include "consts.h"
#include "AtomWorld/AtomWorld.h"
#include "Graphics/Graphics.h"
#include "Physics/Physics.h"
#include "utils/Rand.h"
#include "utils/ThreadsTest.h"
#include "utils/ThreadPool.h"
#include "PerformanceMonitor/PerformanceMonitor.h"
#include "addons/ExplosionHandler.h"
#include "addons/Shooter.h"
#include "addons/RandomSpawner.h"
#include "addons/Chain.h"
#include "addons/Benchmark.h"
#include "addons/Square.h"
#include "Camera/Camera.h"


int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowBounds.getWidth(), windowBounds.getHeight()), "Verlet",
                            sf::Style::Default, settings);
    window.setFramerateLimit(60);

    ThreadPool threadPool{numThreads};
    AtomWorld atomWorld{worldBounds};
    Camera camera{atomWorld.getBoundsF().getWidth(), atomWorld.getBoundsF().getHeight(), Vector2::fromCartesian(0, 0)};
    PerformanceMonitor performanceMonitor = PerformanceMonitor{window, atomWorld};
    Graphics graphics{atomWorld, window, threadPool, performanceMonitor, camera};
    Physics physics{atomWorld, threadPool, performanceMonitor};
    ExplosionHandler explosionHandler{atomWorld};
    Shooter shooter{Vector2::fromCartesian(200, 200), Angle::fromDegrees(45), 4, 1, 4, atomWorld, 1};
    RNGf gen = RNGf(seed);
    RandomSpawner randomSpawner{atomWorld, gen};
    Benchmark benchmark{60 * 30};

    randomSpawner.spawn(15);


    while (window.isOpen()) {

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
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

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                shooter.pointTo(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                       static_cast<float>(event.mouseButton.y)));
                shooter.shoot();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                explosionHandler.launch(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                               static_cast<float>(event.mouseButton.y)), 4, 150);
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace) {
                atomWorld.clear();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M) {
                randomSpawner.spawn(5000);
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

        benchmark.sample();
    }


    return 0;
}
