#pragma once

#include "../Engine/BaseObject.h"
#include "../Engine/InputHandler.h"
#include "../Engine/Camera.h"
#include "../addons/Shooter.h"

class Player : public BaseObject {
public:
    InputHandler &inputHandler;
    Shooter &shooter;
    Scene &scene;
    float movementSpeed = 2000;
    bool movingUp = false;
    bool movingDown = false;
    bool movingLeft = false;
    bool movingRight = false;
    bool isBraking = false;

    int keyPressedListenerId = -1;
    int keyReleasedListenerId = -1;
    int mouseButtonPressedListenerId = -1;
    int mouseWheelScrolledListenerId = -1;

    Player(InputHandler &inputHandler, Shooter &shooter, Scene &scene) :
            inputHandler(inputHandler),
            shooter(shooter),
            scene(scene) {
        // i can't setup events from constructor, because lambda will capture
        // `this` before std::make_unique will fire, which will invalidate `this`, because ownership is transferred to unique_ptr
    }

    ~Player() override {
        inputHandler.removeEventListener(sf::Event::KeyPressed, keyPressedListenerId);
        inputHandler.removeEventListener(sf::Event::KeyReleased, keyReleasedListenerId);
        inputHandler.removeEventListener(sf::Event::MouseButtonPressed, mouseButtonPressedListenerId);
        inputHandler.removeEventListener(sf::Event::MouseWheelScrolled, mouseWheelScrolledListenerId);
    }

    void onInit() override {
        std::cout << "onInit" << std::endl;
        keyPressedListenerId = inputHandler.addEventListener(sf::Event::KeyPressed, [this](sf::Event &event) {
            if (event.key.code == sf::Keyboard::W) {
                movingUp = true;
            }
            if (event.key.code == sf::Keyboard::S) {
                movingDown = true;
            }
            if (event.key.code == sf::Keyboard::A) {
                movingLeft = true;
            }
            if (event.key.code == sf::Keyboard::D) {
                movingRight = true;
            }
            if (event.key.code == sf::Keyboard::Space) {
                isBraking = true;
            }
        });
        keyReleasedListenerId = inputHandler.addEventListener(sf::Event::KeyReleased, [this](sf::Event &event) {
            if (event.key.code == sf::Keyboard::W) {
                movingUp = false;
            }
            if (event.key.code == sf::Keyboard::S) {
                movingDown = false;
            }
            if (event.key.code == sf::Keyboard::A) {
                movingLeft = false;
            }
            if (event.key.code == sf::Keyboard::D) {
                movingRight = false;
            }
            if (event.key.code == sf::Keyboard::Space) {
                isBraking = false;
            }
        });
        mouseButtonPressedListenerId = inputHandler.addEventListener(sf::Event::MouseButtonPressed,
                                                                     [this](sf::Event &event) {
                                                                         if (event.mouseButton.button ==
                                                                             sf::Mouse::Left) {
                                                                             shooter.setPosition(basicDetails->posCurr);
                                                                             shooter.shoot(scene.getCamera().screenPosToWorldPos(
                                                                                     Vector2F::cart(
                                                                                             static_cast<float>(event.mouseButton.x),
                                                                                             static_cast<float>(event.mouseButton.y))));
                                                                         }
                                                                     });
        inputHandler.addEventListener(sf::Event::MouseWheelScrolled, [&](sf::Event &event) {
            if (event.mouseWheelScroll.delta > 0) {
                scene.getCamera().zoom(1.5);
            } else {
                scene.getCamera().zoom(0.75);
            }
        });
    }

    void onTick() override {
        if (!basicDetails) {
            throw std::runtime_error("basicDetails is nullptr");
        }
        scene.getCamera().setPosition((basicDetails->posCurr * 0.2 + scene.getCamera().position * 1.8) / 2);
        if (movingUp) {
//            setVelocity(Vector2F::cart(0, -movementSpeed));
            basicDetails->accelerate(Vector2F::cart(0, -movementSpeed));
        }
        if (movingDown) {
//            setVelocity(Vector2F::cart(0, movementSpeed));
            basicDetails->accelerate(Vector2F::cart(0, movementSpeed));
        }
        if (movingLeft) {
//            setVelocity(Vector2F::cart(-movementSpeed, 0));
            basicDetails->accelerate(Vector2F::cart(-movementSpeed, 0));
        }
        if (movingRight) {
//            setVelocity(Vector2F::cart(movementSpeed, 0));
            basicDetails->accelerate(Vector2F::cart(movementSpeed, 0));
        }
        if (isBraking) {
            basicDetails->setVelocity(basicDetails->getVelocity() * 0.95);
        }
    }

    void onCollision(BaseObject *ptr) override {
        scene.forEachInRadius(basicDetails->posCurr, 50, [&](BaseObject *ptr, int ind) {
            if (ptr != this) {
                scene.removeObject(ind);
            }
        });
    }
};