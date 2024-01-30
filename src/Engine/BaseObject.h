#pragma once


#include "SFML/Graphics/Color.hpp"
#include "EngineConsts.h"

class BaseObject;

class BasicDetails {
public:
    BaseObject* m_parent = nullptr;
    Vector2F m_posCurr, m_posOld, m_acceleration = Vector2F::cart();
    sf::Color m_color = sf::Color::White;
    bool m_isPinned = false;

    void setVelocity(Vector2F v) {
        m_posOld = m_posCurr - v;
    }

    [[nodiscard]] Vector2F getVelocity()const {
        return m_posCurr - m_posOld;
    }

    void addVelocity(Vector2F v) {
        setVelocity(getVelocity() + v);
    }

    void accelerate(Vector2F a) {
        m_acceleration += a;
    }

    void pin() {
        m_isPinned = true;
    }
    void unpin() {
        m_isPinned = false;
    }

//    void update(float dt) {
//        Vector2F velocity = m_posCurr - m_posOld;
//        velocity *= engineDefaults::m_linearDamping;
//        // TODO review maby limiting not needed
//        velocity.limitMagnitude(engineDefaults::m_maxVelocity);
//
//        m_posOld = m_posCurr;
//        m_posCurr += velocity + (m_acceleration * (dt * dt));
//
//        m_acceleration = Vector2F::cart();
//    }

    explicit BasicDetails(Vector2F position) : m_posCurr(position), m_posOld(position) {
    };

    void log() const {
        std::cout << "Pos curr: ";
        m_posCurr.log();
        std::cout << "Pos old: ";
        m_posOld.log();
        std::cout << "Velocity: ";
        getVelocity().log();
        std::cout << "Acceleration: ";
        m_acceleration.log();
        std::cout << "Is pinned: " << m_isPinned << std::endl;
        std::cout << "Color: " << m_color.toInteger() << std::endl;
    }
};

class Scene;

class BaseObject {
private:
    BasicDetails* m_basicDetails = nullptr;
    Scene* m_scene = nullptr;
public:
    [[nodiscard]] BasicDetails& getBasicDetails() const {
        return *m_basicDetails;
    }
    [[nodiscard]] Scene& getScene() const {
        return *m_scene;
    }
    virtual void v_onTick() = 0;
    virtual void v_onInit() = 0;
    virtual void v_onCollision(BaseObject* ptr) = 0;

    virtual ~BaseObject() = default;

    friend class Scene;
};