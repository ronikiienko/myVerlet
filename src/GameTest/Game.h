#pragma once

#include "../Engine/BaseGame.h"
#include "Levels/Level1.h"
#include "Levels/MainMenu.h"
#include "Levels/PerformanceTest.h"

class Game : public BaseGame {
private:
    EBHandle setLevel1EventHandle;
public:
    Game() : BaseGame() {}

    void v_onInit() override {
        std::cout << "Game init" << std::endl;
        setLevel<Level1>();

        setLevel1EventHandle = m_eventBus.addEventListener<SetLevel1Event>([this](const SetLevel1Event& event){
            setLevel<Level1>();
        });
    }

    void v_onTick() override {}
};