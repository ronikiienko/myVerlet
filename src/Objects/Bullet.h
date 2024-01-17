#pragma once


#include "../Engine/BaseObject.h"
#include "../addons/ExplosionHandler.h"

class Bullet : public BaseObject {
public:
    bool m_isExploded = false;
    ExplosionHandler &m_explosionHandler;

    explicit Bullet(ExplosionHandler &explosionHandler) : m_explosionHandler(explosionHandler) {

    }

    void v_onCollision(BaseObject* ptr) override {
//        std::cout << "v_onCollision" << std::endl;
        if (!m_isExploded) {
            m_isExploded = true;
            m_explosionHandler.launch(m_basicDetails->m_posCurr + Vector2F::cart(5, 5), 7, 100);
        }
    }

    void v_onTick() override {};
    void v_onInit() override {};
};