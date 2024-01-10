#pragma once

#include "../Engine/Scene.h"
#include "../Engine/EngineConsts.h"
#include "../Engine/utils/Rand.h"

class RandomSpawner {
private:
    Scene &scene;
    RNGf &gen;
public:
    RandomSpawner(Scene &scene, RNGf &gen) : scene(scene), gen(gen) {}

    void spawn(int number) {
        for (int i = 0; i < number; i++) {
            auto ptr = scene.addObject(

                    BaseObject(),
                    Vector2F::cart
                            (
                                    gen.getInRange
                                            (
                                                    0,
                                                    scene.getSizeF().x
                                            ),
                                    gen.getInRange
                                            (0, scene.getSizeF().y
                                            )
                            )
            );

            BaseObject &object = *ptr.lock();
            object.basicDetails->color = sf::Color(
                    static_cast<int>(object.basicDetails->posCurr.x / scene.getSizeF().x * 255),
                    static_cast<int>(object.basicDetails->posCurr.y / scene.getSizeF().y * 255), 255);
        }
    }
};