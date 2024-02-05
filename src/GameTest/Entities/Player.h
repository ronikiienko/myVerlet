#pragma once

#include "../../Engine/BaseObject/BaseObject.h"
#include "../../Engine/InputBus/InputBus.h"
#include "../../Engine/Camera.h"
#include "../../Engine/Modules/Shooter.h"
#include "./Bullet.h"

class Player : public BaseObject {
public:
    InputBus &m_inputBus;
    RNGf &m_gen;
    sf::RenderWindow &m_window;
    float m_acceleration = 100;
    bool m_movingUp = false;
    bool m_movingDown = false;
    bool m_movingLeft = false;
    bool m_movingRight = false;
    bool m_isBraking = false;
    bool m_isShooting = false;

    bool m_isManualCamera = false;
    Vector2F m_manualCameraPos = Vector2F::cart(0, 0);

    Shooter m_shooter{m_scene, m_gen};

    IBHandle m_keyPressedListener;
    IBHandle m_keyReleasedListener;
    IBHandle m_mouseWheelScrolledListener;
    IBHandle m_mouseButtonPressedListener;
    IBHandle m_mouseButtonReleasedListener;
    IBHandle m_mouseMoveListener;

    Player(ObjectContext context, InputBus &inputBus, sf::RenderWindow &window, RNGf &gen) :
            m_inputBus(inputBus), m_window(window), m_gen(gen), BaseObject(context) {
        // i can't setup events from constructor, because lambda will capture
        // `this` before std::make_unique will fire, which will invalidate `this`, because ownership is transferred to unique_ptr
    }

    void v_onInit() override {
        std::cout << "v_onInit" << std::endl;
        m_keyPressedListener = m_inputBus.addEventListener(sf::Event::KeyPressed, [this](const sf::Event &event) {
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
        m_keyReleasedListener = m_inputBus.addEventListener(sf::Event::KeyReleased, [this](const sf::Event &event) {
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
        m_mouseWheelScrolledListener = m_inputBus.addEventListener(sf::Event::MouseWheelScrolled,
                                                                   [&](const sf::Event &event) {
                                                                       if (event.mouseWheelScroll.delta > 0) {
                                                                           m_scene.getCamera().zoom(1.5);
                                                                       } else {
                                                                           m_scene.getCamera().zoom(0.75);
                                                                       }
                                                                   });
        m_mouseButtonPressedListener = m_inputBus.addEventListener(sf::Event::MouseButtonPressed,
                                                                   [this](const sf::Event &event) {
                                                                       if (event.mouseButton.button ==
                                                                           sf::Mouse::Left) {
                                                                           m_isShooting = true;
                                                                       }
                                                                       if (event.mouseButton.button ==
                                                                           sf::Mouse::Right) {
                                                                           m_isManualCamera = true;
                                                                           m_manualCameraPos = m_scene.getCamera().screenPosToWorldPos(
                                                                                   Vector2F::cart(
                                                                                           event.mouseButton.x,
                                                                                           event.mouseButton.y)
                                                                           );
                                                                       }
                                                                   });
        m_mouseButtonReleasedListener = m_inputBus.addEventListener(sf::Event::MouseButtonReleased,
                                                                    [this](const sf::Event &event) {
                                                                        if (event.mouseButton.button ==
                                                                            sf::Mouse::Left) {
                                                                            m_isShooting = false;
                                                                        }
                                                                        if (event.mouseButton.button ==
                                                                            sf::Mouse::Right) {
                                                                            m_isManualCamera = false;
                                                                        }
                                                                    });
        m_mouseMoveListener = m_inputBus.addEventListener(sf::Event::MouseMoved, [this](const sf::Event &event) {
            getBasicDetails().m_direction =( m_scene.getCamera().screenPosToWorldPos(Vector2F::cart(event.mouseMove.x, event.mouseMove.y)) - getBasicDetails().m_posCurr).normalize();
        });

        m_scene.toggleObjectRotation(this, true);
    }

    void v_onTick() override {
        m_shooter.tick();
        if (m_isShooting) {
            sf::Vector2<int> mousePosition = sf::Mouse::getPosition(m_window);

            m_scene.lineTrace(
                    getBasicDetails().m_posCurr,
                    m_scene.getCamera().screenPosToWorldPos(Vector2F::cart(mousePosition.x, mousePosition.y)),
                    [this](BaseObject *obj, int ind) {
                        if (obj != this) {
                            obj->destroy();
                        }
                    }
            );

//            m_shooter.tryShoot(getBasicDetails().m_posCurr,
//                               m_scene.getCamera().screenPosToWorldPos(
//                                       Vector2F::cart(mousePosition.x, mousePosition.y)),
//                               Bullet{m_scene.getObjectContext()});
        }
        Vector2F newCameraPos = Vector2F::cart(0, 0);
        if (m_isManualCamera) {
            newCameraPos = m_manualCameraPos;
        } else {
            newCameraPos = getBasicDetails().m_posCurr;
        }

        m_scene.getCamera().setPosition((newCameraPos + m_scene.getCamera().getPosition()) / 2);
        if (m_movingUp) {
//            setVelocity(Vector2F::cart(0, -m_acceleration));
            getBasicDetails().accelerate(Vector2F::cart(0, -m_acceleration));
        }
        if (m_movingDown) {
//            setVelocity(Vector2F::cart(0, m_acceleration));
            getBasicDetails().accelerate(Vector2F::cart(0, m_acceleration));
        }
        if (m_movingLeft) {
//            setVelocity(Vector2F::cart(-m_acceleration, 0));
            getBasicDetails().accelerate(Vector2F::cart(-m_acceleration, 0));
        }
        if (m_movingRight) {
//            setVelocity(Vector2F::cart(m_acceleration, 0));
            getBasicDetails().accelerate(Vector2F::cart(m_acceleration, 0));
        }
        if (m_isBraking) {
            getBasicDetails().setVelocity(getBasicDetails().getVelocity() * 0.95);
        }
    }

    void v_onCollision(BaseObject *ptr) override {
//        m_scene.forEachInRadius(getBasicDetails().m_posCurr, 50, [&](BaseObject *ptr, int ind) {
//            if (ptr != this) {
//                m_scene.removeObject(ind);
//            }
//        });
    }
};