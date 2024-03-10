#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../consts.h"
#include "../../Engine/Modules/ExplosionHandler.h"
#include "../../Engine/Modules/RandomPositionGenerator.h"
#include "../../Engine/Modules/CameraControls.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    SMHandle m_firstSound;
    IBHandle m_inputHandle;
    CameraControls m_move{m_scene, m_inputBus};
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {

    }

    void v_onTick() override {
    }
};