#pragma once

#include "Engine/BaseGame.h"
#include "Level.h"

class Game : public BaseGame {
public:
    Game() : BaseGame() {
        setLevel<Level>();
    }
};