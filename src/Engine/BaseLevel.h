#pragma once


#include "Scene.h"
#include "Camera.h"
#include "InputHandler.h"
#include "EventBus.h"
#include "SoundManager.h"
#include "TimerManager.h"

struct LevelContext {
    sf::RenderWindow &window;
    ThreadPool &threadPool;
    EventBus &eventBus;
    SoundManager &soundManager;
    TimerManager &timerManager;
    InputHandler &inputHandler;

    LevelContext(sf::RenderWindow &window, ThreadPool &threadPool, EventBus &eventBus, SoundManager &soundManager,
                 TimerManager &timerManager, InputHandler &inputHandler)
            : window(window), threadPool(threadPool), eventBus(eventBus), soundManager(soundManager),
              timerManager(timerManager), inputHandler(inputHandler) {}
};

class BaseLevel {
protected:
    Scene scene;
    Graphics graphics;
    Physics physics;
    PerformanceMonitor performanceMonitor;
    sf::RenderWindow &window;
    ThreadPool &threadPool;
    EventBus &eventBus;
    InputHandler &inputHandler;
    SoundManager &soundManager;
    TimerManager &timerManager;
public:

    explicit BaseLevel(
            LevelContext levelContext,
            int maxObjectsNum = engineDefaults::maxObjectsNum,
            Vector2I worldSize = engineDefaults::worldSize
    ) :
            window(levelContext.window),
            threadPool(levelContext.threadPool),
            eventBus(levelContext.eventBus),
            soundManager(levelContext.soundManager),
            timerManager(levelContext.timerManager),
            inputHandler(levelContext.inputHandler),
            scene(
                    Camera{
                            engineDefaults::cameraLongestDimViewSize,
                            engineDefaults::cameraDefaultPosition,
                            levelContext.window,
                            levelContext.inputHandler
                    },
                    levelContext.threadPool,
                    performanceMonitor,
                    maxObjectsNum,
                    worldSize
            ),
            graphics(scene, levelContext.window,
                     levelContext.threadPool,
                     performanceMonitor),
            physics(scene, levelContext.threadPool,
                    performanceMonitor),
            performanceMonitor(levelContext.window) {


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