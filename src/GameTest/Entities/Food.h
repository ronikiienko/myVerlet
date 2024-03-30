#pragma once


#include "../../Engine/BaseObject/BaseObject.h"

class Food : public BaseObject {
public:
    explicit Food(ObjectContext context) : BaseObject(context) {}

    void v_onTick() override {};
    void v_onInit() override {
        getBasicDetails().m_color = sf::Color::Magenta;
        getBasicDetails().pin();
    };
    void v_onCollision(BaseObject* ptr) override {};
};