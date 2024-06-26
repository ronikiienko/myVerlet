#pragma once


#include "Scene/Scene.h"
#include "Camera.h"
#include "InputBus/InputBus.h"
#include "TimerManager/TimerManager.h"
#include "EventBus/EventBus.h"
#include "SoundManager/SoundManager.h"
#include "DebugGraphics.h"
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

struct LevelContext {
    sf::RenderWindow &m_window;
    ThreadPool &m_threadPool;
    EventBus &m_eventBus;
    SoundManager &m_soundManager;
    TimerManager &m_timerManager;
    InputBus &m_inputBus;
    PerformanceMonitor &m_performanceMonitor;
    tgui::Gui &m_gui;


    LevelContext(sf::RenderWindow &window, ThreadPool &threadPool, EventBus &eventBus, SoundManager &soundManager,
                 TimerManager &timerManager, InputBus &inputBus, PerformanceMonitor &performanceMonitor, tgui::Gui &gui)
            : m_window(window), m_threadPool(threadPool), m_eventBus(eventBus), m_soundManager(soundManager),
              m_timerManager(timerManager), m_inputBus(inputBus), m_performanceMonitor(performanceMonitor),
              m_gui(gui) {}
};

class BaseLevel {
private:
    void update() {
        m_performanceMonitor.start("removingMarkedObjects");
        m_scene.getObjectStorage().removeMarkedObjects();
        m_performanceMonitor.end("removingMarkedObjects");
        m_performanceMonitor.start("removingMarkedSticks");
        m_scene.getStickStorage().removeMarkedSticks();
        m_performanceMonitor.end("removingMarkedSticks");

        m_performanceMonitor.start("physics");
        m_physics.update();
        m_performanceMonitor.end("physics");

        m_window.clear(sf::Color::Black);

        m_performanceMonitor.start("graphics");
        m_graphics.update();
        m_performanceMonitor.end("graphics");
        m_performanceMonitor.start("debugGraphics");
        m_debugGraphics.update();
        m_performanceMonitor.end("debugGraphics");

        m_performanceMonitor.start("gui");
        m_gui.draw();
        m_performanceMonitor.end("gui");

        m_window.display();
        m_scene.runTicks();

        m_performanceMonitor.start("level tick");
        tick();
        m_performanceMonitor.end("level tick");
    }

    void init() {
        v_onInit();
        tgui::Label::Ptr debugWidget = tgui::Label::create();
        debugWidget->setTextSize(12);
        debugWidget->setPosition(5.0f, 5.0f);
        debugWidget->getRenderer()->setTextColor(sf::Color::White);
        m_gui.add(debugWidget, "debugWidget");
    }

    void tick() {
        v_onTick();
        m_gui.get<tgui::Label>("debugWidget")->setText(m_performanceMonitor.getString());
    }

    IBHandle windowResizeHandle;
protected:
    Scene m_scene;
    Graphics m_graphics;
    DebugGraphics m_debugGraphics;
    Physics m_physics;
    PerformanceMonitor &m_performanceMonitor;
    sf::RenderWindow &m_window;
    tgui::Gui &m_gui;
    ThreadPool &m_threadPool;
    EventBus &m_eventBus;
    InputBus &m_inputBus;
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
            m_inputBus(levelContext.m_inputBus),
            m_performanceMonitor(levelContext.m_performanceMonitor),
            m_gui(levelContext.m_gui),
            m_scene(
                    cameraMaxWorldViewSize,
                    Vector2F::cart(0, 0),
                    levelContext.m_inputBus,
                    levelContext.m_window,
                    levelContext.m_threadPool,
                    levelContext.m_performanceMonitor,
                    maxObjectsNum,
                    worldSize
            ),
            m_graphics(m_scene, levelContext.m_window,
                       levelContext.m_threadPool,
                       levelContext.m_performanceMonitor),
            m_debugGraphics(m_scene, levelContext.m_window, levelContext.m_threadPool,
                            levelContext.m_performanceMonitor),
            m_physics(m_scene, levelContext.m_threadPool,
                      levelContext.m_performanceMonitor) {


    }

    void toggleDebugWidget() {
        setDebugWidgetEnabled(!m_gui.get<tgui::Label>("debugWidget")->isVisible());
    }

    void setDebugWidgetEnabled(bool enabled) {
        if (enabled) {
            m_gui.get<tgui::Label>("debugWidget")->setVisible(true);
        } else {
            m_gui.get<tgui::Label>("debugWidget")->setVisible(false);
        }
    }

public:
    virtual void v_onInit() = 0;

    virtual void v_onTick() = 0;

    BaseLevel(const BaseLevel &) = delete;

    BaseLevel(BaseLevel &&) = delete;

    BaseLevel &operator=(const BaseLevel &) = delete;

    BaseLevel &operator=(BaseLevel &&) = delete;

    virtual ~BaseLevel() = default;

    friend class BaseGame;
};