#pragma once

#include "../Engine/BaseObject.h"
#include "../Engine/CallbackSystems/InputBus.h"
#include "../Engine/Camera.h"
#include "../addons/Shooter.h"

class Player : public BaseObject {
public:
    InputBus &m_inputHandler;
    Scene &m_scene;
    float m_acceleration = 100;
    bool m_movingUp = false;
    bool m_movingDown = false;
    bool m_movingLeft = false;
    bool m_movingRight = false;
    bool m_isBraking = false;

    int m_keyPressedListenerId = -1;
    int m_keyReleasedListenerId = -1;
    int m_mouseButtonPressedListenerId = -1;
    int m_mouseWheelScrolledListenerId = -1;

    Player(InputBus &inputHandler, Scene &scene) :
            m_inputHandler(inputHandler),
            m_scene(scene) {
        // i can't setup events from constructor, because lambda will capture
        // `this` before std::make_unique will fire, which will invalidate `this`, because ownership is transferred to unique_ptr
    }

    ~Player() override {
        m_inputHandler.removeEventListener(sf::Event::KeyPressed, m_keyPressedListenerId);
        m_inputHandler.removeEventListener(sf::Event::KeyReleased, m_keyReleasedListenerId);
        m_inputHandler.removeEventListener(sf::Event::MouseButtonPressed, m_mouseButtonPressedListenerId);
        m_inputHandler.removeEventListener(sf::Event::MouseWheelScrolled, m_mouseWheelScrolledListenerId);
    }

    void v_onInit() override {
        std::cout << "v_onInit" << std::endl;
        m_keyPressedListenerId = m_inputHandler.addEventListener(sf::Event::KeyPressed, [this](const sf::Event &event) {
            if (event.key.code == sf::Keyboard::W) {
                m_movingUp = true;
            }
            if (event.key.code == sf::Keyboard::S) {
                m_movingDown = true;
            }
            if (event.key.code == sf::Keyboard::A) {
                m_movingLeft = true;
            }
            if (event.key.code == sf::Keyboard::D) {
                m_movingRight = true;
            }
            if (event.key.code == sf::Keyboard::Space) {
                m_isBraking = true;
            }
        });
        m_keyReleasedListenerId = m_inputHandler.addEventListener(sf::Event::KeyReleased, [this](const sf::Event &event) {
            if (event.key.code == sf::Keyboard::W) {
                m_movingUp = false;
            }
            if (event.key.code == sf::Keyboard::S) {
                m_movingDown = false;
            }
            if (event.key.code == sf::Keyboard::A) {
                m_movingLeft = false;
            }
            if (event.key.code == sf::Keyboard::D) {
                m_movingRight = false;
            }
            if (event.key.code == sf::Keyboard::Space) {
                m_isBraking = false;
            }
        });
        m_inputHandler.addEventListener(sf::Event::MouseWheelScrolled, [&](const sf::Event &event) {
            if (event.mouseWheelScroll.delta > 0) {
                m_scene.getCamera().zoom(1.5);
            } else {
                m_scene.getCamera().zoom(0.75);
            }
        });
    }

    void v_onTick() override {
        if (!m_basicDetails) {
            throw std::runtime_error("m_basicDetails is nullptr");
        }
//        m_scene.getCamera().setPosition((m_basicDetails->m_posCurr * 0.2 + m_scene.getCamera().m_worldCenterPos * 1.8) / 2);
        m_scene.getCamera().setPosition(m_basicDetails->m_posCurr);
        if (m_movingUp) {
//            setVelocity(Vector2F::cart(0, -m_acceleration));
            m_basicDetails->accelerate(Vector2F::cart(0, -m_acceleration));
        }
        if (m_movingDown) {
//            setVelocity(Vector2F::cart(0, m_acceleration));
            m_basicDetails->accelerate(Vector2F::cart(0, m_acceleration));
        }
        if (m_movingLeft) {
//            setVelocity(Vector2F::cart(-m_acceleration, 0));
            m_basicDetails->accelerate(Vector2F::cart(-m_acceleration, 0));
        }
        if (m_movingRight) {
//            setVelocity(Vector2F::cart(m_acceleration, 0));
            m_basicDetails->accelerate(Vector2F::cart(m_acceleration, 0));
        }
        if (m_isBraking) {
            m_basicDetails->setVelocity(m_basicDetails->getVelocity() * 0.95);
        }
    }

    void v_onCollision(BaseObject *ptr) override {
//        m_scene.forEachInRadius(m_basicDetails->m_posCurr, 50, [&](BaseObject *ptr, int ind) {
//            if (ptr != this) {
//                m_scene.removeObject(ind);
//            }
//        });
    }
};