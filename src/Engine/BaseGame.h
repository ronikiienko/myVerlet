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
#include "EngineConsts.h"


class BaseGame {
protected:
    sf::RenderWindow m_window = sf::RenderWindow(sf::VideoMode(engineDefaults::windowSize.m_x,
                                                               engineDefaults::windowSize.m_y), "Verlet",
                                                 sf::Style::Default, sf::ContextSettings(0, 0, 1));
    tgui::Gui m_gui{m_window};
    ThreadPool m_threadPool;

    std::unique_ptr<BaseLevel> m_level;
    // TODO maby move to m_level because levels may leave their thrash behind (m_callbacks)
    EventBus m_eventBus;
    InputHandler m_inputHandler{m_window};
    SoundManager m_soundManager;
    TimerManager m_timerManager;
    PerformanceMonitor m_performanceMonitor;
public:
    explicit BaseGame(int numThreads = engineDefaults::numThreads) : m_threadPool(numThreads) {
        m_window.setFramerateLimit(60);
    }

    template<typename T>
    void setLevel() {
        m_inputHandler.clear();
        m_gui.removeAllWidgets();
        std::unique_ptr<T> ptr = std::make_unique<T>(
                LevelContext(m_window, m_threadPool, m_eventBus, m_soundManager, m_timerManager, m_inputHandler,
                             m_performanceMonitor, m_gui));
        m_level = std::move(ptr);
        m_level->init();
    }

    void init() {
        v_onInit();
        if (!m_level) {
            throw std::runtime_error("Level should be set in v_onInit() method");
        }

        m_inputHandler.addEventListener(sf::Event::Resized, [&](const sf::Event &event) {
            sf::View view = sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width),
                                                   static_cast<float>(event.size.height)));
            m_window.setView(view);
        });
        while (m_window.isOpen()) {
            m_performanceMonitor.start("total");
            m_level->update();
            m_performanceMonitor.start("input");
            sf::Event event{};
            while (m_window.pollEvent(event)) {
                bool isConsumed = m_gui.handleEvent(event);
                if(!isConsumed) {
                    m_inputHandler.update(event);
                }
            }
            m_performanceMonitor.end("input");
            v_onTick();

            m_performanceMonitor.start("timer manager");
            m_timerManager.tick();
            m_performanceMonitor.end("timer manager");

            m_performanceMonitor.end("total");
        }
    }

    virtual void v_onInit() = 0;

    virtual void v_onTick() = 0;

    BaseGame(const BaseGame &) = delete;

    BaseGame(BaseGame &&) = delete;

    BaseGame &operator=(const BaseGame &) = delete;

    BaseGame &operator=(BaseGame &&) = delete;

    virtual ~BaseGame() = default;
};