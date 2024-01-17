#pragma once

#include "../Engine/BaseObject.h"

class EmptyObject : public BaseObject {
public:
    explicit EmptyObject() : BaseObject() {}

    void onTick() override {};
    void onInit() override {};
    void onCollision(BaseObject* ptr) override {};
};