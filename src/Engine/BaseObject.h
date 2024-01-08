#pragma once


#include "SFML/Graphics/Color.hpp"
#include "EngineConsts.h"

class BaseObject;

class BasicDetails {
public:
    BaseObject* parent = nullptr;
    Vector2 posCurr, posOld, acceleration = Vector2::cart();
    sf::Color color = sf::Color::White;
    bool isPinned = false;

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

        acceleration = Vector2::cart();
    }

    explicit BasicDetails(Vector2 position) : posCurr(position), posOld(position) {
    };

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


class BaseObject {
public:
    BasicDetails* basicDetails = nullptr;

    virtual void onTick() {};
    virtual void onInit() {};
    virtual void onCollision(BaseObject* ptr) {};

    virtual ~BaseObject() = default;
};