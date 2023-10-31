#pragma once

#include "../modules/Vector.h"
#include "../World/World.h"
#include "../consts.h"

class Square {
public:
    Square(World& world, Vector2 startPos, float size) {
        int ind1 = world.addObject(startPos, objectsRadius);
        int ind2 = world.addObject(startPos + Vector2::fromCartesian(size,0), objectsRadius);
        int ind3 = world.addObject(startPos + Vector2::fromCartesian(size,size), objectsRadius);
        int ind4 = world.addObject(startPos + Vector2::fromCartesian(0,size), objectsRadius);

        world.addStick(ind1, ind2);
        world.addStick(ind2, ind3);
        world.addStick(ind3, ind4);
        world.addStick(ind4, ind1);
        world.addStick(ind1, ind3);
        world.addStick(ind4, ind2);
    }
};