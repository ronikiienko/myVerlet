#pragma once

#include "../../Scene/BaseStick/BaseStick.h"

class EmptyStick: public BaseStick {
    void v_onInit() override {
        getBasicStickDetails().m_color = sf::Color::Red;
    };
    void v_onTick() override {

    };
};