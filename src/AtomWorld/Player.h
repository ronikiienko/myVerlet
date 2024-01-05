#pragma once

#include "VerletObject.h"
#include "../InputHandler/InputHandler.h"

class Player : public VerletObject {
public:
    InputHandler &inputHandler;
    Camera &camera;
    Shooter &shooter;

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
                addVelocity(Vector2::fromCartesian(0, -0.5));
            }
            if (event.key.code == sf::Keyboard::S) {
                addVelocity(Vector2::fromCartesian(0, 0.5));
            }
            if (event.key.code == sf::Keyboard::A) {
                addVelocity(Vector2::fromCartesian(-0.5, 0));
            }
            if (event.key.code == sf::Keyboard::D) {
                addVelocity(Vector2::fromCartesian(0.5, 0));
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
    }
};