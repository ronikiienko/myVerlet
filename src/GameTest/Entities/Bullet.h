#pragma once

#include "../../Engine/BaseObject.h"
#include "../../Engine/Scene.h"

class Bullet : public BaseObject {
    Scene& m_scene;
    int m_ticksToLive = 100;
    int m_hitsLeft = 10;
public:
    explicit Bullet(Scene& scene);
    void v_onInit() override;
    void v_onCollision(BaseObject *ptr) override;
    void v_onTick() override;
};