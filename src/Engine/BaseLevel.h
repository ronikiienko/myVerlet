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

    LevelContext(sf::RenderWindow &window, ThreadPool &threadPool, EventBus &eventBus, SoundManager &soundManager,
                 TimerManager &timerManager, InputHandler &inputHandler)
            : m_window(window), m_threadPool(threadPool), m_eventBus(eventBus), m_soundManager(soundManager),
              m_timerManager(timerManager), m_inputHandler(inputHandler) {}
};

class BaseLevel {
protected:
    Scene m_scene;
    Graphics m_graphics;
    Physics m_physics;
    PerformanceMonitor m_performanceMonitor;
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
            Vector2I worldSize = engineDefaults::worldSize
    ) :
            m_window(levelContext.m_window),
            m_threadPool(levelContext.m_threadPool),
            m_eventBus(levelContext.m_eventBus),
            m_soundManager(levelContext.m_soundManager),
            m_timerManager(levelContext.m_timerManager),
            m_inputHandler(levelContext.m_inputHandler),
            m_scene(
                    Camera{
                            engineDefaults::cameraLongestDimViewSize,
                            engineDefaults::cameraDefaultPosition,
                            levelContext.m_window,
                            levelContext.m_inputHandler
                    },
                    levelContext.m_threadPool,
                    m_performanceMonitor,
                    maxObjectsNum,
                    worldSize
            ),
            m_graphics(m_scene, levelContext.m_window,
                       levelContext.m_threadPool,
                       m_performanceMonitor),
            m_physics(m_scene, levelContext.m_threadPool,
                      m_performanceMonitor),
            m_performanceMonitor(levelContext.m_window) {


    }

    void update() {
        m_performanceMonitor.start("total");

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

        m_performanceMonitor.start("onTick");
        m_scene.runTick();
        onTick();
        m_performanceMonitor.end("onTick");

        m_performanceMonitor.start("removingMarked");
        m_scene.removeMarkedObjects();
        m_performanceMonitor.end("removingMarked");

        m_performanceMonitor.setObjectsCount(m_scene.getObjectsCount());
        m_performanceMonitor.end("total");
    }

    virtual void onInit() {}

    virtual void onTick() {}
};