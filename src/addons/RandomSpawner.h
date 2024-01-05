#pragma once

#include "../AtomWorld/AtomWorld.h"
#include "../utils/Rand.h"
#include "../consts.h"

class RandomSpawner {
private:
    AtomWorld &atomWorld;
    RNGf &gen;
public:
    RandomSpawner(AtomWorld &atomWorld, RNGf &gen) : atomWorld(atomWorld), gen(gen) {}

    void spawn(int number) {
        for (int i = 0; i < number; i++) {
            int objInt = atomWorld.addObject(

                    VerletObject
                            (
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
                            )
            );

            VerletObject &object = atomWorld.getObject(objInt);
            object.color = sf::Color(static_cast<int>(object.posCurr.x / consts::worldBounds.getWidth() * 255),
                                     static_cast<int>(object.posCurr.y / consts::worldBounds.getHeight() * 255), 255);
        }
    }
};