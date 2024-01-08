#pragma once

#include "../Engine/AtomWorld.h"
#include "../Engine/EngineConsts.h"
#include "../Engine/utils/Rand.h"

class RandomSpawner {
private:
    AtomWorld &atomWorld;
    RNGf &gen;
public:
    RandomSpawner(AtomWorld &atomWorld, RNGf &gen) : atomWorld(atomWorld), gen(gen) {}

    void spawn(int number) {
        for (int i = 0; i < number; i++) {
            auto ptr = atomWorld.addObject(

                    BaseObject(),
                    Vector2F::cart
                            (
                                    gen.getInRange
                                            (
                                                    0,
                                                    atomWorld.getBoundsF().getWidth()
                                            ),
                                    gen.getInRange
                                            (0, atomWorld.getBoundsF().getHeight()
                                            )
                            )
            );

            BaseObject &object = *ptr.lock();
            object.basicDetails->color = sf::Color(static_cast<int>(object.basicDetails->posCurr.x / consts::worldBounds.getWidth() * 255),
                                     static_cast<int>(object.basicDetails->posCurr.y / consts::worldBounds.getHeight() * 255), 255);
        }
    }
};