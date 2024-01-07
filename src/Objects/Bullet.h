#pragma once


#include "../Engine/BaseObject.h"
#include "../addons/ExplosionHandler.h"

class Bullet : public BaseObject {
public:
    bool isExploded = false;
    ExplosionHandler &explosionHandler;

    explicit Bullet(ExplosionHandler &explosionHandler) : explosionHandler(explosionHandler) {

    }

    void onCollision() override {
//        std::cout << "onCollision" << std::endl;
        if (!isExploded) {
            isExploded = true;
            explosionHandler.launch(basicDetails->posCurr + Vector2::fromCartesian(5,5), 7, 100);
        }
    }
};