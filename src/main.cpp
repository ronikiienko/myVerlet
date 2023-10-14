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

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowBounds.getWidth(), windowBounds.getHeight()), "Verlet", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    ThreadPool threadPool{numThreads};
    World world{worldBounds};
    Graphics graphics{world, window, threadPool};
    Physics physics{world, threadPool};

    RNGf gen = RNGf(seed);

    for (int i = 0; i < maxObjectNum; i++) {
        VerletObject& object = world.addObject(
                Vector2::fromCartesian(gen.getInRange(0, worldBounds.getWidth()), gen.getInRange(0, worldBounds.getHeight())),
                gen.getInRange(minRadius, maxRadius)
        );
         object.color = sf::Color(static_cast<int>(object.posCurr.x / worldBounds.getWidth() * 255), static_cast<int>(object.posCurr.y / worldBounds.getHeight() * 255), 255);
    }


    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        if (logFps) {
            const double elapsed = clock.restart().asMilliseconds();
            std::cout << "FPS: " << 1000 / elapsed << '\n';
        }

        physics.update();
        graphics.update();
//        sf::Clock clock1;
//        const long long elapsedk = clock1.restart().asMicroseconds();
//        std::cout << "elapsed: " << elapsedk << '\n';

    }


    return 0;
}
