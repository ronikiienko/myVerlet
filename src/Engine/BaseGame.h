#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "EngineConsts.h"
#include "utils/ThreadPool.h"
#include "Scene.h"
#include "Graphics.h"
#include "Physics.h"
#include "InputHandler.h"
#include "BaseLevel.h"

class BaseGame {
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(consts::windowSize.x,
                                                             consts::windowSize.y), "Verlet",
                                               sf::Style::Default, sf::ContextSettings(0, 0, 1));
    ThreadPool threadPool{consts::numThreads};
    Scene scene{consts::worldSize, Camera{static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y),
                                      Vector2F::cart(1200, 900)}};
    Graphics graphics{scene, window, threadPool, performanceMonitor};
    Physics physics{scene, threadPool, performanceMonitor};
    PerformanceMonitor performanceMonitor{window, scene};
    InputHandler inputHandler{window};
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
            window.display();
            performanceMonitor.end("graphics");

            performanceMonitor.start("input");
            inputHandler.update();
            performanceMonitor.end("input");

            performanceMonitor.start("onTick");
            scene.runTick();
            performanceMonitor.end("onTick");

            performanceMonitor.start("removingMarked");
            scene.removeMarkedObjects();
            performanceMonitor.end("removingMarked");

            performanceMonitor.end("total");
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
        scene.clear();
        std::unique_ptr<T> ptr = std::make_unique<T>(LevelContext(scene, inputHandler));
        level = std::move(ptr);
        level->onInit();
    }
};