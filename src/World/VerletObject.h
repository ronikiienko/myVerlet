#pragma once

#include "../modules/Vector.h"
#include "SFML/Graphics/Color.hpp"


class VerletObject {
public:
    Vector2 posCurr, posOld, acceleration = Vector2::fromCartesian();
    sf::Color color = sf::Color::White;
    bool isPinned = false;

    explicit VerletObject(Vector2 position) : posCurr(position), posOld(position) {};

    void setVelocity(Vector2 v) {
        posOld = posCurr - v;
    }

    [[nodiscard]] Vector2 getVelocity()const {
        return posCurr - posOld;
    }

    void addVelocity(Vector2 v) {
        setVelocity(getVelocity() + v);
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
        velocity *= linearDamping;
        // TODO review maby limiting not needed
        velocity.limitMagnitude(maxVelocity);

        posOld = posCurr;
        posCurr += velocity + (acceleration * (dt * dt));

        acceleration = Vector2::fromCartesian();
    }
};