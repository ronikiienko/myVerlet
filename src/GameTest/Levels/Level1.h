#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../../addons/RandomSpawner.h"
#include "../Entities/Player.h"
#include "../../addons/Shooter.h"
#include "../../consts.h"
#include "../Entities/Enemy.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    Shooter m_shooter{Vector2F::cart(200, 200), 2, 1, 4, m_scene, 1, m_explosionHandler};
    RNGf m_gen{consts::seed};
    RandomSpawner m_randomSpawner{m_scene, m_gen};
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        m_physics.setMaxVelocity(1000);
        m_randomSpawner.spawn(15000);
        auto playerPtr = m_scene.addObject(Player{m_inputBus, m_scene}, Vector2F::cart(100, 100));
        for (int i = 0; i < 10; i++) {
            m_scene.addObject(Enemy{dynamic_cast<Player*>(playerPtr.lock().get())}, Vector2F::cart(m_gen.getInRange(0, 2000), m_gen.getInRange(0, 2000)));
        }
        std::cout << "Objects num: " << m_scene.getObjectsCount() << std::endl;
    }
    void v_onTick() override {}
};