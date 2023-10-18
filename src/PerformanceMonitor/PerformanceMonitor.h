#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"

class PerformanceMonitor {
private:
    sf::RenderWindow& window;
    std::unordered_map<std::string, sf::Clock> clocks;
    std::unordered_map<std::string, long long> times;
    sf::Font font;
    sf::Text text;
public:
    explicit PerformanceMonitor(sf::RenderWindow& window): window(window) {
        if(!font.loadFromFile("../res/Roboto-Medium.ttf")) {
            throw std::runtime_error("Font could not load. Perhaps it doesn't exist?");
        }
        text.setFont(font);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(10);
        text.setPosition(200.0f, 200.0f);
        text.setString("hiii");

    }
    void start(const std::string& label) {
        if (!clocks.count(label)) {
            clocks.insert({label, sf::Clock{}});
        }
        sf::Clock& clock = clocks.find(label)->second;
        clock.restart();
    }
    void end(const std::string& label) {
        if (!clocks.count(label)) {
            throw std::runtime_error("Trying to end timer that wasn't started (PerformanceMonitor)" + label);
        }
        sf::Clock& clock = clocks.find(label)->second;
        const long long elapsed = clock.restart().asMilliseconds();

        if (!times.count(label)) {
            times.insert({label, 0.0f});
        }
        times[label] = elapsed;
    }
    void remove(const std::string& label) {
        clocks.erase(label);
        times.erase(label);
    }

    void draw() {
        window.draw(text);
        window.display();
    }
};
