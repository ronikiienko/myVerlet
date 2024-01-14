#pragma once


#include "Scene.h"
#include "Camera.h"
#include "InputHandler.h"
#include "EventBus.h"
#include "SoundManager.h"
#include "TimerManager.h"

struct LevelContext {
    sf::RenderWindow &m_window;
    ThreadPool &m_threadPool;
    EventBus &m_eventBus;
    SoundManager &m_soundManager;
    TimerManager &m_timerManager;
    InputHandler &m_inputHandler;
    PerformanceMonitor& m_performanceMonitor;

    LevelContext(sf::RenderWindow &window, ThreadPool &threadPool, EventBus &eventBus, SoundManager &soundManager,
                 TimerManager &timerManager, InputHandler &inputHandler, PerformanceMonitor& performanceMonitor)
            : m_window(window), m_threadPool(threadPool), m_eventBus(eventBus), m_soundManager(soundManager),
              m_timerManager(timerManager), m_inputHandler(inputHandler), m_performanceMonitor(performanceMonitor) {}
};

class BaseLevel {
protected:
    Scene m_scene;
    Graphics m_graphics;
    Physics m_physics;
    PerformanceMonitor& m_performanceMonitor;
    sf::RenderWindow &m_window;
    ThreadPool &m_threadPool;
    EventBus &m_eventBus;
    InputHandler &m_inputHandler;
    SoundManager &m_soundManager;
    TimerManager &m_timerManager;
public:

    explicit BaseLevel(
            LevelContext levelContext,
            int maxObjectsNum = engineDefaults::maxObjectsNum,
            Vector2I worldSize = engineDefaults::worldSize,
            float cameraMaxWorldViewSize = engineDefaults::cameraMaxWorldViewSize
    ) :
            m_window(levelContext.m_window),
            m_threadPool(levelContext.m_threadPool),
            m_eventBus(levelContext.m_eventBus),
            m_soundManager(levelContext.m_soundManager),
            m_timerManager(levelContext.m_timerManager),
            m_inputHandler(levelContext.m_inputHandler),
            m_performanceMonitor(levelContext.m_performanceMonitor),
            m_scene(
                    cameraMaxWorldViewSize,
                    Vector2F::cart(0, 0),
                    levelContext.m_inputHandler,
                    levelContext.m_window,
                    levelContext.m_threadPool,
                    levelContext.m_performanceMonitor,
                    maxObjectsNum,
                    worldSize
            ),
            m_graphics(m_scene, levelContext.m_window,
                       levelContext.m_threadPool,
                       levelContext.m_performanceMonitor),
            m_physics(m_scene, levelContext.m_threadPool,
                      levelContext.m_performanceMonitor) {


    }

    void update() {
        m_performanceMonitor.start("m_physics");
        m_physics.update();
        m_performanceMonitor.end("m_physics");

        m_performanceMonitor.start("m_graphics");
        m_window.clear(sf::Color::Black);
        m_graphics.update();
        m_performanceMonitor.draw();
        m_window.display();
        m_performanceMonitor.end("m_graphics");

        m_performanceMonitor.start("input");
        m_inputHandler.update();
        m_performanceMonitor.end("input");

        m_performanceMonitor.start("object ticks");
        m_scene.runObjectTicks();
        onTick();
        m_performanceMonitor.end("object ticks");

        m_performanceMonitor.start("removingMarked");
        m_scene.removeMarkedObjects();
        m_performanceMonitor.end("removingMarked");

        m_performanceMonitor.setObjectsCount(m_scene.getObjectsCount());
    }

    virtual void onInit() {}

    virtual void onTick() {}

    BaseLevel(const BaseLevel&) = delete;
    BaseLevel(BaseLevel&&) = delete;
    BaseLevel& operator=(const BaseLevel&) = delete;
    BaseLevel& operator=(BaseLevel&&) = delete;
};