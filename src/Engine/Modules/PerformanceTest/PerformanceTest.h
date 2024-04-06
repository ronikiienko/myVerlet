#pragma once

#include "../../BaseLevel.h"
#include "../../utils/Rand.h"
#include "../ExplosionHandler.h"
#include "EmptyObject.h"
#include "../RandomPositionGenerator.h"
#include "../CameraControls.h"

class PerformanceTest : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{10};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    CameraControls m_cameraControls{m_scene, m_inputBus};
public:
    explicit PerformanceTest(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        for (int i = 0; i < 150000; i++) {
            m_scene.getObjectStorage().addObject(EmptyObject{m_scene.getObjectContext()}, m_randomPositionGenerator.get());
        };
    }
    void v_onTick() override {}
};