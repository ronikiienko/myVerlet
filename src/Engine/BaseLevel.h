#pragma once


#include "Scene.h"
#include "Camera.h"
#include "InputHandler.h"
#include "EventBus.h"
#include "SoundManager.h"
#include "TimerManager.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

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
private:
    void update() {
        m_performanceMonitor.start("physics");
        m_physics.update();
        m_performanceMonitor.end("physics");

        m_window.clear(sf::Color::Black);

        m_performanceMonitor.start("graphics");
        m_graphics.update();
        m_performanceMonitor.end("graphics");

        m_performanceMonitor.start("gui");
        m_performanceMonitor.draw();
        m_gui.draw();
        m_performanceMonitor.end("gui");

        m_window.display();

        m_performanceMonitor.start("object ticks");
        m_scene.runObjectTicks();
        m_performanceMonitor.end("object ticks");

        m_performanceMonitor.start("level tick");
        onTick();
        m_performanceMonitor.end("level tick");

        m_performanceMonitor.start("removingMarked");
        m_scene.removeMarkedObjects();
        m_performanceMonitor.end("removingMarked");
    }

    void init() {
        onInit();
    }
protected:
    Scene m_scene;
    Graphics m_graphics;
    Physics m_physics;
    PerformanceMonitor& m_performanceMonitor;
    sf::RenderWindow &m_window;
    tgui::Gui m_gui{m_window};
    ThreadPool &m_threadPool;
    EventBus &m_eventBus;
    InputHandler &m_inputHandler;
    SoundManager &m_soundManager;
    TimerManager &m_timerManager;

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
public:
    virtual void onInit() = 0;
    virtual void onTick() = 0;

    BaseLevel(const BaseLevel&) = delete;
    BaseLevel(BaseLevel&&) = delete;
    BaseLevel& operator=(const BaseLevel&) = delete;
    BaseLevel& operator=(BaseLevel&&) = delete;

    friend class BaseGame;
};