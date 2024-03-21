#pragma once

#include "../Engine/BaseGame.h"
#include "Levels/Level1.h"
#include "../Engine/Modules/PerformanceTest/PerformanceTest.h"

class Game : public BaseGame {
private:
    EBHandle setLevel1EventHandle;
public:
    Game() : BaseGame() {}

    void v_onInit() override {
        std::cout << "Game init" << std::endl;
        setLevel<Level1>();
    }

    void v_onTick() override {}
};