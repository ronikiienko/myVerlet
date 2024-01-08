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

    std::unique_ptr<BaseLevel> level;


public:
    BaseGame() {
        window.setFramerateLimit(60);
    }

    template<typename T>
    void setLevel() {
        std::unique_ptr<T> ptr = std::make_unique<T>(LevelContext(window, threadPool));
        level = std::move(ptr);
        level->launch();
    }
};