#pragma once


#include "../Engine/BaseObject.h"
#include "../addons/ExplosionHandler.h"

class Bullet : public BaseObject {
public:
    bool isExploded = false;
    ExplosionHandler &explosionHandler;

    explicit Bullet(ExplosionHandler &explosionHandler) : explosionHandler(explosionHandler) {

    }

    void onCollision(BaseObject* ptr) override {
//        std::cout << "onCollision" << std::endl;
        if (!isExploded) {
            isExploded = true;
            explosionHandler.launch(basicDetails->posCurr + Vector2F::cart(5, 5), 7, 100);
        }
    }
};