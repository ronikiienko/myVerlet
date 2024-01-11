#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "SFML/System/Clock.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "Scene.h"

class PerformanceMonitor {
private:
    sf::RenderWindow& m_window;
    std::unordered_map<std::string, sf::Clock> m_clocks;
    std::unordered_map<std::string, long long> m_times;
    sf::Font m_font;
    sf::Text m_text;
    int m_objectsCount = 0;
public:
    explicit PerformanceMonitor(sf::RenderWindow& window): m_window(window) {
        if(!m_font.loadFromFile("./res/Roboto-Medium.ttf")) {
            throw std::runtime_error("Font could not load. Perhaps it doesn't exist?");
        }
        m_text.setFont(m_font);
        m_text.setFillColor(sf::Color::White);
        m_text.setCharacterSize(12);
        m_text.setPosition(5.0f, 5.0f);
    }
    void start(const std::string& label) {
        if (!m_clocks.count(label)) {
            m_clocks.insert({label, sf::Clock{}});
        }
        sf::Clock& clock = m_clocks.find(label)->second;
        clock.restart();
    }
    void end(const std::string& label) {
        if (!m_clocks.count(label)) {
            throw std::runtime_error("Trying to end timer that wasn't started (PerformanceMonitor)" + label);
        }
        sf::Clock& clock = m_clocks.find(label)->second;
        const long long elapsed = clock.restart().asMicroseconds();

        if (!m_times.count(label)) {
            m_times.insert({label, 0.0f});
        }
        m_times[label] = elapsed;
    }
    void remove(const std::string& label) {
        m_clocks.erase(label);
        m_times.erase(label);
    }

    void draw() {
        std::string textString;
        textString += "Objects count:  " + std::to_string(m_objectsCount) + "\n";
        for (auto& it : m_times) {
            double milliseconds = static_cast<double>(it.second) / 1000;
            short fps = 1000 / milliseconds;
            textString += '\n' + it.first + ":   " + std::to_string(milliseconds) + " ms" + "    /    " + std::to_string(fps) + " FPS";
        }
        m_text.setString(textString);
        m_window.draw(m_text);
    }

    void setObjectsCount(int count) {
        m_objectsCount = count;
    }
};

