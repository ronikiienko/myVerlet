#pragma once

#include <Backend/SFML-Graphics.hpp>
#include <Widgets/Label.hpp>
#include "../Engine/EventBus/EventBus.h"
#include "Events.h"

class UiRenderer {
private:
    EventBus &m_eventBus;
    tgui::Gui &m_gui;
    EBHandle m_playerBulletCountUpdate;
    EBHandle m_enemyCountUpdate;
public:
    UiRenderer(EventBus &eventBus, tgui::Gui &gui) : m_eventBus(eventBus), m_gui(gui) {
        m_gui.loadWidgetsFromFile("C:\\Users\\ronik\\CLionProjects\\myVerlet\\src\\GameTest\\bullet.txt");
        m_playerBulletCountUpdate = m_eventBus.addEventListener<GameEvents::PlayerBulletCountUpdate>([this](const GameEvents::PlayerBulletCountUpdate& event){
            tgui::Label::Ptr bulletCountLabel = m_gui.get<tgui::Label>("bulletCount");
            bulletCountLabel->setText(std::to_string(event.m_newCount));
        });
        m_enemyCountUpdate = m_eventBus.addEventListener<GameEvents::EnemyCountUpdate>([this](const GameEvents::EnemyCountUpdate& event){
            tgui::Label::Ptr enemyCountLabel = m_gui.get<tgui::Label>("enemyCount");
            enemyCountLabel->setText(std::to_string(event.m_newCount));
        });
    }
};