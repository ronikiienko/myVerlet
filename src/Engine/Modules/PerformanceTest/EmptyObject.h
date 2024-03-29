#pragma once

#include "../../BaseObject/BaseObject.h"

class EmptyObject : public BaseObject {
public:
    explicit EmptyObject(ObjectContext context) : BaseObject(context) {}

    void v_onTick() override {};
    void v_onInit() override {};
    void v_onCollision(BaseObject* ptr) override {};
};