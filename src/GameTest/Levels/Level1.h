#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../../addons/RandomSpawner.h"
#include "../Entities/Player.h"
#include "../consts.h"
#include "../Entities/Enemy.h"
#include "../../Engine/Modules/ExplosionHandler.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomSpawner m_randomSpawner{m_scene, m_gen};
    SMHandle m_firstSound;
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        m_randomSpawner.spawn(150000);
        auto playerPtr = m_scene.addObject(Player{m_scene.getObjectContext(), m_inputBus, m_window, m_gen},
                                           Vector2F::cart(102, 102));
//        for (int i = 0; i < 1000; i++) {
//            m_scene.addObject(Enemy{m_scene.getObjectContext(), dynamic_cast<Player*>(playerPtr.lock().get()), m_gen}, Vector2F::cart(m_gen.getInRange(0, 2000), m_gen.getInRange(0, 2000)));
//        }
        m_firstSound = m_soundManager.play("./res/ride.wav");
    }

    void v_onTick() override {}
};