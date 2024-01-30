#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../../Engine/Scene.h"

class Bullet : public BaseObject {
    int m_ticksToLive = 100;
    int m_hitsLeft = 10;
public:
    void v_onInit() override;
    void v_onCollision(BaseObject *ptr) override;
    void v_onTick() override;

    explicit Bullet(ObjectContext context);
};