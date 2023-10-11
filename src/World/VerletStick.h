#pragma once

#include "VerletObject.h"

class VerletStick {
private:
    VerletObject &obj1;
    VerletObject &obj2;
    float length;
public:
    VerletStick(VerletObject &obj1, VerletObject &obj2) : obj1(obj1), obj2(obj2) {
        length = (obj1.posCurr - obj2.posCurr).magnitude();
    }

    void constraint() const {
        const Vector2 vectorBetween = obj1.posCurr - obj2.posCurr;
        const float distanceBetween = vectorBetween.magnitude();

        const float diff = distanceBetween - length;
        const float moveRatio = (diff / distanceBetween) / 2;

        const Vector2 offset = vectorBetween * moveRatio;

        if (!obj1.isPinned) obj1.posCurr += offset;
        if (!obj2.isPinned) obj2.posCurr -= offset;
    }
};