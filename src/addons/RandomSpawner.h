#pragma once

#include "../World/World.h"
#include "../modules/Rand.h"
#include "../consts.h"

class RandomSpawner {
private:
    World &world;
    RNGf &gen;
public:
    RandomSpawner(World &world, RNGf &gen) : world(world), gen(gen) {}

    void spawn(int number) {
        for (int i = 0; i < number; i++) {
            int objInt = world.addObject(
                    Vector2::fromCartesian(gen.getInRange(0, world.getBoundsF().getWidth()),
                                           gen.getInRange(0, world.getBoundsF().getHeight()))
            );
            VerletObject &object = world.getObject(objInt);
            object.color = sf::Color(static_cast<int>(object.posCurr.x / worldBounds.getWidth() * 255),
                                     static_cast<int>(object.posCurr.y / worldBounds.getHeight() * 255), 255);
        }
    }
};