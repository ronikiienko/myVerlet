#pragma once

#include "../../Engine/BaseObject/BaseObject.h"

class EmptyObject : public BaseObject {
public:
    explicit EmptyObject(ObjectContext context) : BaseObject(context) {}

    void v_onTick() override {};
    void v_onInit() override {};
};