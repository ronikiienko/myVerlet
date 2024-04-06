#pragma once


#include "SFML/Graphics/Color.hpp"
#include "../EngineConsts.h"

class BaseObject;

class BasicDetails {
public:
    BaseObject* m_parent = nullptr;
    Vector2F m_posCurr, m_posOld, m_acceleration = Vector2F::cart();
    sf::Color m_color = sf::Color::White;
    bool m_isPinned = false;
    float m_mass = 1;
    bool m_isCollisionOn = true;
    Vector2F m_direction = Vector2F::polar(1, Angle::fromDegrees(0));

    void setVelocity(Vector2F v) {
        m_posOld = m_posCurr - v;
    }

    [[nodiscard]] const Vector2F getVelocity()const {
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
//        velocity.limitMagnitudeModify(engineDefaults::m_maxVelocity);
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

struct ObjectContext {
    explicit ObjectContext(Scene& scene) : m_scene(scene) {};
    Scene& m_scene;
};

class BaseObject {
private:
    BasicDetails* m_basicDetails = nullptr;
public:
    Scene& m_scene;
    [[nodiscard]] BasicDetails& getBasicDetails() const {
//        if (m_scene == nullptr) throw std::runtime_error("Getting scene but it's not initialized yet. Call this method only after or inside onInit()");
        return *m_basicDetails;
    };
    virtual void v_onTick() = 0;
    virtual void v_onInit() = 0;

    // Important note: this method can be called many times for same collision. It doesn't check if objects were colliding in previous frame.
    // Also, it can be called after object is destroyed (because destruction is delayed to the end of the frame, so during subSteps it can be called).
    virtual void v_onCollision(BaseObject* ptr) = 0;

    void destroy();
    void toggleRotation(bool enabled);

    virtual ~BaseObject() = default;

    explicit BaseObject(ObjectContext);

    friend class ObjectStorage;
};