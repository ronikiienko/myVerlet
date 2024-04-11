#pragma once

#include "../../Engine/Scene/BaseObject/BaseObject.h"
#include "../../Engine/InputBus/InputBus.h"
#include "../../Engine/Modules/Shooter.h"
#include "Bullet.h"
#include "../../Engine/Modules/CameraFollowing.h"
#include "../Events.h"

class Player : public BaseObject {
private:
    InputBus &m_inputBus;

    IBHandle m_keyPressHandle;
    IBHandle m_keyReleaseHandle;
    IBHandle m_mousePressHandle;
    IBHandle m_mouseReleaseHandle;

    float m_speed = 0.02;

    bool m_isMovingRight = false;
    bool m_isMovingLeft = false;
    bool m_isMovingUp = false;
    bool m_isMovingDown = false;

    bool m_isShooting = false;

    RNGf &m_gen;

    Shooter m_shooter;

    CameraFollowing m_cameraFollowing;

    int m_bulletsLeft = 1000;

    EventBus &m_eventBus;
public:
    explicit Player(InputBus &inputBus, RNGf &gen, EventBus &eventBus, Shooter &shooter,
                    CameraFollowing &cameraFollowing) : m_inputBus(inputBus), m_shooter(shooter), m_gen(gen),
                                                        m_eventBus(eventBus),
                                                        m_cameraFollowing(cameraFollowing) {}

    void v_onTick() override {
        m_cameraFollowing.follow(getBasicDetails().m_posCurr);
        Vector2F acceleration = Vector2F::cart();
        if (m_isMovingRight) {
            acceleration.m_x += m_speed;
        }
        if (m_isMovingLeft) {
            acceleration.m_x -= m_speed;
        }
        if (m_isMovingUp) {
            acceleration.m_y -= m_speed;
        }
        if (m_isMovingDown) {
            acceleration.m_y += m_speed;
        }
        getBasicDetails().accelerate(acceleration);
        m_shooter.tick();
        if (m_isShooting && m_bulletsLeft > 0) {
            bool didShoot = m_shooter.tryShoot(getBasicDetails().m_posCurr, getScene().getCamera().screenPosToWorldPos(
                    sf::Mouse::getPosition(getScene().getCamera().getWindow())), Bullet{});
            if (didShoot) {
                m_bulletsLeft--;
                m_eventBus.emit(GameEvents::PlayerBulletCountUpdate{m_bulletsLeft});
            }
        }
    };

    void v_onInit() override {
        m_cameraFollowing.setTransitionCoefficient(0.1);
        m_shooter.setSpread(Angle::fromDegrees(5));
        m_shooter.setCooldown(20);
        m_shooter.setSpeed(0.8);
        getBasicDetails().m_color = sf::Color::Green;
        m_keyPressHandle = m_inputBus.addEventListener(sf::Event::KeyPressed, [this](sf::Event event) {
            if (event.key.code == sf::Keyboard::D) {
                m_isMovingRight = true;
            }
            if (event.key.code == sf::Keyboard::A) {
                m_isMovingLeft = true;
            }
            if (event.key.code == sf::Keyboard::W) {
                m_isMovingUp = true;
            }
            if (event.key.code == sf::Keyboard::S) {
                m_isMovingDown = true;
            }
        });
        m_keyReleaseHandle = m_inputBus.addEventListener(sf::Event::KeyReleased, [this](sf::Event event) {
            if (event.key.code == sf::Keyboard::D) {
                m_isMovingRight = false;
            }
            if (event.key.code == sf::Keyboard::A) {
                m_isMovingLeft = false;
            }
            if (event.key.code == sf::Keyboard::W) {
                m_isMovingUp = false;
            }
            if (event.key.code == sf::Keyboard::S) {
                m_isMovingDown = false;
            }
        });
        m_mousePressHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                m_isShooting = true;
            }
        });
        m_mouseReleaseHandle = m_inputBus.addEventListener(sf::Event::MouseButtonReleased, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                m_isShooting = false;
            }
        });
    };

    void v_onCollision(BaseObject *ptr) override {};
};