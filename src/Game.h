#pragma once

#include "Engine/BaseGame.h"
#include "Level.h"

class Game : public BaseGame {
public:
    Game() : BaseGame() {}

    void onInit() override {
        std::cout << "Game init" << std::endl;
        setLevel<Level>();
    }
};