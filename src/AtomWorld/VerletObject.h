#pragma once

#include "../utils/Vector.h"
#include "SFML/Graphics/Color.hpp"
#include "../consts.h"


class VerletObject {
public:
    Vector2 posCurr, posOld, acceleration = Vector2::fromCartesian();
    sf::Color color = sf::Color::White;
    bool isPinned = false;

    explicit VerletObject(Vector2 position) : posCurr(position), posOld(position) {
    };

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
        velocity *= consts::linearDamping;
        // TODO review maby limiting not needed
        velocity.limitMagnitude(consts::maxVelocity);

        posOld = posCurr;
        posCurr += velocity + (acceleration * (dt * dt));

        acceleration = Vector2::fromCartesian();
    }

    void log() const {
        std::cout << "Pos curr: ";
        posCurr.log();
        std::cout << "Pos old: ";
        posOld.log();
        std::cout << "Velocity: ";
        getVelocity().log();
        std::cout << "Acceleration: ";
        acceleration.log();
        std::cout << "Is pinned: " << isPinned << std::endl;
        std::cout << "Color: " << color.toInteger() << std::endl;
    }
};