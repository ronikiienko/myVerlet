#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../../addons/RandomSpawner.h"
#include "../Entities/Player.h"
#include "../consts.h"
#include "../Entities/Enemy.h"
#include "../../Engine/Modules/ExplosionHandler.h"

class PerformanceTest : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomSpawner m_randomSpawner{m_scene, m_gen};
public:
    explicit PerformanceTest(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        m_randomSpawner.spawn(150000);
        auto playerPtr = m_scene.addObject(Player{m_scene.getObjectContext(), m_inputBus, m_window, m_gen}, Vector2F::cart(100, 100));
    }
    void v_onTick() override {}
};