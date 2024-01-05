#pragma once

#include "VerletObject.h"
#include "../InputHandler/InputHandler.h"

class Player : public VerletObject {
public:
    InputHandler &inputHandler;
    Camera &camera;
    Shooter &shooter;
    float movementSpeed = 1000;
    bool movingUp = false;
    bool movingDown = false;
    bool movingLeft = false;
    bool movingRight = false;
    Player(Vector2 position, InputHandler &inputHandler, Camera &camera, Shooter &shooter) : VerletObject(position),
                                                                                             inputHandler(inputHandler),
                                                                                             camera(camera),
                                                                                             shooter(shooter) {
        // i can't setup events from constructor, because lambda will capture
        // `this` before std::make_unique will fire, which will invalidate `this`, because ownership is transferred to unique_ptr
    }

    void onInit() override {
        std::cout << "onInit" << std::endl;
        inputHandler.addEventListener(sf::Event::KeyPressed, [this](sf::Event &event) {
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
        });
        inputHandler.addEventListener(sf::Event::KeyReleased, [this](sf::Event &event) {
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
        });
        inputHandler.addEventListener(sf::Event::MouseButtonPressed, [this](sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                shooter.setPosition(posCurr);
                shooter.shoot(camera.screenPosToWorldPos(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                                                static_cast<float>(event.mouseButton.y))));
            }
        });
    }

    void onTick() override {
        camera.setPosition(posCurr);
        if (movingUp) {
//            setVelocity(Vector2::fromCartesian(0, -movementSpeed));
            accelerate(Vector2::fromCartesian(0, -movementSpeed));
        }
        if (movingDown) {
//            setVelocity(Vector2::fromCartesian(0, movementSpeed));
            accelerate(Vector2::fromCartesian(0, movementSpeed));
        }
        if (movingLeft) {
//            setVelocity(Vector2::fromCartesian(-movementSpeed, 0));
            accelerate(Vector2::fromCartesian(-movementSpeed, 0));
        }
        if (movingRight) {
//            setVelocity(Vector2::fromCartesian(movementSpeed, 0));
            accelerate(Vector2::fromCartesian(movementSpeed, 0));
        }
    }
};