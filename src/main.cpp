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

int iterations = 100000000;

void testVectorSubtractionSpeed() {
    sf::Vector2f a(1.0f, 2.0f), b(3.0f, 4.0f);

    sf::Vector2f result(0.0f, 0.0f);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        result = a - b;
        a.x = result.x;
        a.y = result.y;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken for " << iterations << " iterations: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Final result: (" << result.x << ", " << result.y << ")"
              << std::endl;  // Output result to prevent optimization
}
void testCustomVectorSubtractionSpeed() {
    Vector2 a = Vector2::fromCartesian(1.0f, 2.0f);
    Vector2 b = Vector2::fromCartesian(3.0f, 4.0f);

    Vector2 result = Vector2::fromCartesian(0.0f, 0.0f);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        result = a - b;
        a.x = result.x;
        a.y = result.y;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken for " << iterations << " iterations: " << duration.count() << " milliseconds" << std::endl;
    result.log();  // Output result to prevent optimization
}


int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(windowDims.x, windowDims.y), "Verlet", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    World world{RectangleI::fromSize(0, 0, worldDims.x, worldDims.y)};
    Graphics graphics{world, window};
    Physics physics{world};

    RNGf gen = RNGf();

    for (int i = 0; i < 2500; i++) {
        world.addObject(
                Vector2::fromCartesian(gen.getInRange(0, worldDims.x), gen.getInRange(0, worldDims.y)),
                gen.getInRange(2, 5)
        );
    }

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        const double elapsed = clock.restart().asMilliseconds();
        std::cout << "FPS: " << 1000 / elapsed << '\n';

        physics.update();
        graphics.update();
    }


    return 0;
}
