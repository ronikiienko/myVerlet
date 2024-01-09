#pragma once


#include "Scene.h"
#include "Camera.h"
#include "InputHandler.h"
#include "EventBus.h"

struct LevelContext {
    sf::RenderWindow &window;
    ThreadPool &threadPool;
    EventBus &eventBus;

    LevelContext(sf::RenderWindow &window, ThreadPool &threadPool, EventBus& eventBus) : window(window), threadPool(threadPool), eventBus(eventBus) {}
};

class BaseLevel {
protected:
    Scene scene;
    Graphics graphics;
    Physics physics;
    PerformanceMonitor performanceMonitor;
    InputHandler inputHandler;
    sf::RenderWindow &window;
    ThreadPool &threadPool;
    EventBus &eventBus;
public:

    explicit BaseLevel(LevelContext levelContext) : window(levelContext.window), threadPool(levelContext.threadPool), eventBus(levelContext.eventBus),
                                                    scene(
                                                            consts::worldSize,
                                                            Camera{static_cast<float>(levelContext.window.getSize().x),
                                                                   static_cast<float>(levelContext.window.getSize().y),
                                                                   Vector2F::cart(1200, 900),
                                                            },
                                                            levelContext.threadPool,
                                                            performanceMonitor),
                                                    graphics(scene, levelContext.window, levelContext.threadPool,
                                                             performanceMonitor),
                                                    physics(scene, levelContext.threadPool, performanceMonitor),
                                                    performanceMonitor(levelContext.window),
                                                    inputHandler(levelContext.window) {


    }

    void update() {
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
        onTick();
        performanceMonitor.end("onTick");

        performanceMonitor.start("removingMarked");
        scene.removeMarkedObjects();
        performanceMonitor.end("removingMarked");

        performanceMonitor.setObjectsCount(scene.getObjectsCount());
        performanceMonitor.end("total");
    }

    virtual void onInit() {}

    virtual void onTick() {}
};