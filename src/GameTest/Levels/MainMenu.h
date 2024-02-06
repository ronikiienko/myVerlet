#pragma once

#include "../../Engine/BaseLevel.h"
#include "../Events.h"

class MainMenu : public BaseLevel {
public:
    explicit MainMenu(LevelContext levelContext) : BaseLevel(levelContext) {}

    void v_onTick() override {
    }
    void v_onInit() override {
        m_gui.loadWidgetsFromFile("C:\\Users\\ronik\\CLionProjects\\myVerlet\\cmake-build-debug\\res\\MainMenu.txt");
        m_gui.get<tgui::Button>("level1")->onPress([this](){
            m_eventBus.emit(SetLevel1Event());
        });
    }
};