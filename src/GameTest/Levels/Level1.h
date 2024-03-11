#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../consts.h"
#include "../../Engine/Modules/ExplosionHandler.h"
#include "../../Engine/Modules/RandomPositionGenerator.h"
#include "../../Engine/Modules/CameraControls.h"
#include "../../Engine/Modules/PerformanceTest/EmptyObject.h"
#include "../Entities/Player.h"
#include "../Entities/Enemy.h"
#include "../UiRenderer.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    SMHandle m_firstSound;
    IBHandle m_inputHandle;
    CameraControls m_move{m_scene, m_inputBus};
    UiRenderer uiRenderer{m_eventBus, m_gui};
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        std::weak_ptr<BaseObject> playerGeneralPtr = m_scene.addObject(Player{m_scene.getObjectContext(), m_inputBus, m_gen, m_eventBus}, Vector2F::cart(100, 100));
        std::weak_ptr<Player> playerPtr = std::static_pointer_cast<Player>(playerGeneralPtr.lock());
        for (int i = 0; i < 2; i++) {
            m_scene.addObject(Enemy{m_scene.getObjectContext(), playerPtr, m_gen, m_eventBus}, m_randomPositionGenerator.get());
        }
        for (int i = 0; i < 10; i++) {
            m_scene.addObject(EmptyObject{m_scene.getObjectContext()}, m_randomPositionGenerator.get());
        }
    }

    void v_onTick() override {
    }
};