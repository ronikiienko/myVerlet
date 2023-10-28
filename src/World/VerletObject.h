#pragma once

#include "../modules/Vector.h"
#include "SFML/Graphics/Color.hpp"


class VerletObject {
public:
    Vector2 posCurr, posOld, acceleration = Vector2::fromCartesian();
    sf::Color color = sf::Color::White;
    float radius;
    bool isPinned = false;

    VerletObject(Vector2 position, float radius) : posCurr(position), posOld(position), radius(radius) {};

    void setVelocity(Vector2 v) {
        posOld = posCurr - v;
    }

    void accelerate(Vector2 a) {
        acceleration += a;
    }

    void pin() {
        isPinned = true;
    }
    void unpin() {
        isPinned = false;
    }

    void update(float dt) {
        Vector2 velocity = posCurr - posOld;
        // TODO review maby limiting not needed
        velocity.limitMagnitude(maxVelocity);

        posOld = posCurr;
        posCurr += velocity + (acceleration * (dt * dt));

        acceleration = Vector2::fromCartesian();
    }
};