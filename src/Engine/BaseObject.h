#pragma once


#include "SFML/Graphics/Color.hpp"
#include "EngineConsts.h"

class BaseObject;

class BasicDetails {
public:
    BaseObject* parent = nullptr;
    Vector2F posCurr, posOld, acceleration = Vector2F::cart();
    sf::Color color = sf::Color::White;
    bool isPinned = false;

    void setVelocity(Vector2F v) {
        posOld = posCurr - v;
    }

    [[nodiscard]] Vector2F getVelocity()const {
        return posCurr - posOld;
    }

    void addVelocity(Vector2F v) {
        setVelocity(getVelocity() + v);
    }

    void accelerate(Vector2F a) {
        acceleration += a;
    }

    void pin() {
        isPinned = true;
    }
    void unpin() {
        isPinned = false;
    }

//    void update(float dt) {
//        Vector2F velocity = posCurr - posOld;
//        velocity *= physicsDefs::linearDamping;
//        // TODO review maby limiting not needed
//        velocity.limitMagnitude(physicsDefs::maxVelocity);
//
//        posOld = posCurr;
//        posCurr += velocity + (acceleration * (dt * dt));
//
//        acceleration = Vector2F::cart();
//    }

    explicit BasicDetails(Vector2F position) : posCurr(position), posOld(position) {
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