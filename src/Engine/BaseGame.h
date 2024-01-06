#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "EngineConsts.h"
#include "utils/ThreadPool.h"
#include "AtomWorld.h"
#include "Graphics.h"
#include "Physics.h"
#include "../addons/Benchmark.h"
#include "InputHandler.h"
#include "BaseLevel.h"

class BaseGame {
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(consts::windowBounds.getWidth(),
                                                             consts::windowBounds.getHeight()), "Verlet",
                                               sf::Style::Default, sf::ContextSettings(0, 0, 1));
    ThreadPool threadPool{consts::numThreads};
    AtomWorld atomWorld{consts::worldBounds};
    Graphics graphics{atomWorld, window, threadPool, performanceMonitor, camera};
    Physics physics{atomWorld, threadPool, performanceMonitor};
    Benchmark benchmark{60 * 30};
    PerformanceMonitor performanceMonitor{window, atomWorld};
    InputHandler inputHandler{window};
    Camera camera{static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y),
                  Vector2::fromCartesian(0, 0)};
    std::unique_ptr<BaseLevel> level;

    void startLoop() {
        while (window.isOpen()) {
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

public:
    void initialize() {
        startLoop();
    }
    BaseGame() {
        window.setFramerateLimit(60);
    }

    template<typename T>
    void setLevel() {
        atomWorld.clear();
        std::unique_ptr<T> ptr = std::make_unique<T>(atomWorld, camera, inputHandler);
        level = std::move(ptr);
        level->onInit();
    }
};