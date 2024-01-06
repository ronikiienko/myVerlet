#pragma once


#include "../Engine/BaseObject.h"
#include "../addons/ExplosionHandler.h"

class Bullet : public BaseObject {
public:
    bool isExploded = false;
    ExplosionHandler &explosionHandler;

    explicit Bullet(Vector2 position, ExplosionHandler &explosionHandler) : BaseObject(position), explosionHandler(explosionHandler) {

    }

    void onCollision() override {
//        std::cout << "onCollision" << std::endl;
        if (!isExploded) {
            isExploded = true;
            explosionHandler.launch(posCurr + Vector2::fromCartesian(5,5), 1, 20);
        }
    }
};