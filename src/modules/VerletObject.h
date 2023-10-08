#pragma once

#include "Vector.h"
#include "SFML/Graphics/Color.hpp"

struct PullConfig {
    float force = 0, maxDistance = 0, on = false;
};

class VerletObject {
public:
    Vector2 posCurr, posOld, acceleration = Vector2::fromCartesian();
    sf::Color color = sf::Color::White;
    float radius;
    PullConfig pull;

    VerletObject(Vector2 position, float radius) : posCurr(position), posOld(position), radius(radius) {};

    void setPull(PullConfig pullConfig) {
        pull = pullConfig;
    }

    void setVelocity(Vector2 v) {
        posOld = posCurr - v;
    }

    void accelerate(Vector2 a) {
        acceleration = acceleration + a;
    }

    void update(float dt) {
        const Vector2 velocity = posCurr - posOld;

        posOld = posCurr;
        posCurr = posCurr + velocity + (acceleration * (dt * dt));

        acceleration = Vector2::fromCartesian();
    }
};