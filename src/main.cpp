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

int main() {
    float num1 = 0.00001;
    float num2 = 832.569568;
    float num3 = 102.569568;
    float sum = num1 + num2;
    float sum1 = num1 + num3;


    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowBounds.getWidth(), windowBounds.getHeight()), "Verlet",
                            sf::Style::Default, settings);
    window.setFramerateLimit(60);

    ThreadPool threadPool{numThreads};
    World world{worldBounds};
    Graphics graphics{world, window, threadPool};
    Physics physics{world, threadPool};
    PerformanceMonitor performanceMonitor = PerformanceMonitor{window};

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


    sf::Clock clock;

    while (window.isOpen()) {

        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                for (int i = 0; i < 50; i++) {
                    const int centerX = worldBounds.getX2() / 2;
                    const int centerY = worldBounds.getY2() / 2;
                    VerletObject &newObject = world.addObject(
                            Vector2::fromCartesian(
                                    centerX + gen.getInRange(-20, 20),
                                    centerY + gen.getInRange(-20, 20)
                            ),
                            gen.getInRange(minRadius, maxRadius)
                    );
                    float velocityX = gen.getInRange(-5, 5);
                    float velocityY = gen.getInRange(-5, 5);
                    newObject.setVelocity(Vector2::fromCartesian(velocityX, velocityY));
                }
            }
        }
        if (logFps) {
            const double elapsed = clock.restart().asMilliseconds();
            std::cout << "FPS: " << 1000 / elapsed << '\n';
        }

        physics.update();
        graphics.update();
//        sf::Clock clock1;
//        performanceMonitor.draw();
//        const long long elapsedk = clock1.restart().asMicroseconds();
//        std::cout << "elapsed: " << elapsedk << '\n';

    }


    return 0;
}
