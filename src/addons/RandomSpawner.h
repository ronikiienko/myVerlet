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
            int objInt = atomWorld.addObject(

                    BaseObject(),
                    Vector2::fromCartesian
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

            BaseObject &object = atomWorld.getObject(objInt);
            object.basicDetails->color = sf::Color(static_cast<int>(object.basicDetails->posCurr.x / consts::worldBounds.getWidth() * 255),
                                     static_cast<int>(object.basicDetails->posCurr.y / consts::worldBounds.getHeight() * 255), 255);
        }
    }
};