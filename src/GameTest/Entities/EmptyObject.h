#pragma once

#include "../../Engine/BaseObject/BaseObject.h"

class EmptyObject : public BaseObject {
public:
    explicit EmptyObject() : BaseObject() {}

    void v_onTick() override {};
    void v_onInit() override {};
    void v_onCollision(BaseObject* ptr) override {};
};