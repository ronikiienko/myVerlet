#pragma once

#include "Engine/BaseLevel.h"
#include "addons/ExplosionHandler.h"
#include "addons/Shooter.h"
#include "Engine/utils/Rand.h"
#include "addons/RandomSpawner.h"
#include "Objects/Player.h"

class Level : public BaseLevel {
    ExplosionHandler explosionHandler{atomWorld};
    Shooter shooter{Vector2::fromCartesian(200, 200), 2, 1, 4, atomWorld, 1, explosionHandler};
    RNGf gen{consts::seed};
    RandomSpawner randomSpawner{atomWorld, gen};
public:
    Level(AtomWorld &atomWorld, Camera &camera, InputHandler &inputHandler) : BaseLevel(atomWorld, camera,
                                                                                        inputHandler) {
    }

    void onInit() override {
        std::cout << "DerivedLevel::onInit() called\n";
        int id = atomWorld.addObject(Player{Vector2::fromCartesian(100, 100), inputHandler, camera, shooter});
        inputHandler.addEventListener(sf::Event::MouseWheelScrolled, [&](sf::Event &event) {
            if (event.mouseWheelScroll.delta > 0) {
                camera.zoom(1.5);
            } else {
                camera.zoom(0.75);
            }
        });
        inputHandler.addEventListener(sf::Event::KeyPressed, [&](sf::Event &event) {
            if (event.key.code == sf::Keyboard::BackSpace) {
                atomWorld.clear();
            }

            if (event.key.code == sf::Keyboard::M) {
                randomSpawner.spawn(5000);
            }
        });
        inputHandler.addEventListener(sf::Event::MouseButtonPressed, [&](sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                explosionHandler.launch(
                        camera.screenPosToWorldPos(Vector2::fromCartesian(static_cast<float>(event.mouseButton.x),
                                                                          static_cast<float>(event.mouseButton.y))),
                        4,
                        150);
            }
        });
    }
};