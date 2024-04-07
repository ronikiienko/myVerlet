#pragma once

#include "../../Scene/BaseStick/BaseStick.h"

class EmptyStick: public BaseStick {
    void v_onInit() override {};
    void v_onTick() override {};
};