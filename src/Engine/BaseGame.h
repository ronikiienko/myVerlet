#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "EngineConsts.h"
#include "utils/ThreadPool.h"
#include "Scene.h"
#include "Graphics.h"
#include "Physics.h"
#include "InputHandler.h"
#include "BaseLevel.h"
#include "EventBus.h"
#include "SoundManager.h"
#include "TimerManager.h"

class BaseGame {
protected:
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(engineDefaults::windowSize.x,
                                                             engineDefaults::windowSize.y), "Verlet",
                                               sf::Style::Default, sf::ContextSettings(0, 0, 1));
    ThreadPool threadPool;

    std::unique_ptr<BaseLevel> level;
    // TODO maby move to level because levels may leave their thrash behind (callbacks)
    EventBus eventBus;
    InputHandler inputHandler{window};
    SoundManager soundManager;
    TimerManager timerManager;
public:
    explicit BaseGame(int numThreads = engineDefaults::numThreads) : threadPool(numThreads) {
        window.setFramerateLimit(60);
    }

    template<typename T>
    void setLevel() {
        std::unique_ptr<T> ptr = std::make_unique<T>(LevelContext(window, threadPool, eventBus, soundManager, timerManager, inputHandler));
        level = std::move(ptr);
        level->onInit();
    }

    void init() {
        onInit();
        if (!level) {
            throw std::runtime_error("Level should be set in onInit() method");
        }
        while (window.isOpen()) {
            level->update();
            onTick();
            timerManager.tick();
        }
    }

    virtual void onInit() {}
    virtual void onTick() {}
};