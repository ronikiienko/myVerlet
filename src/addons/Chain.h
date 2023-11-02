#pragma once

#include "../modules/Vector.h"
#include "../World/World.h"
#include "../consts.h"

class Chain {
public:
    Chain(World& world, Vector2 startPos, Vector2 endPos, int length) {
        const Vector2 vectorBetween = endPos - startPos;
        const Vector2 move = vectorBetween / static_cast<float>(length - 1);

        int lastInd = -1;

        for (int i = 0; i < length; i++) {
            int newInd;
            if (lastInd == -1) {
                newInd = world.addObject(startPos);
            } else {
                newInd = world.addObject(world.getObject(lastInd).posCurr + move);
            }
            if (lastInd != -1) world.addStick(lastInd, newInd);
            if (i == 0 || i == length - 1) {
                VerletObject& object = world.getObject(newInd);
                object.pin();
            }
            lastInd = newInd;
        }
    }
};