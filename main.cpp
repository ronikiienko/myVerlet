#include <iostream>
#include <SFML/Graphics.hpp>
#include <chrono>
#include "modules/Vector.h"

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
    Vector2 a(1.0f, 2.0f), b(3.0f, 4.0f);

    Vector2 result(0.0f, 0.0f);

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
    std::cout << "Hello, World!" << '\n';

    testVectorSubtractionSpeed();
    testCustomVectorSubtractionSpeed();

    return 0;
}
