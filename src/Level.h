#pragma once

#include "Engine/BaseLevel.h"
#include "Engine/utils/Rand.h"
#include "addons/RandomSpawner.h"
#include "Objects/Player.h"
#include "addons/Shooter.h"

class Level : public BaseLevel {
    ExplosionHandler explosionHandler{atomWorld};
    Shooter shooter{Vector2::fromCartesian(200, 200), 2, 1, 4, atomWorld, 1, explosionHandler};
    RNGf gen{consts::seed};
    RandomSpawner randomSpawner{atomWorld, gen};
public:
    explicit Level(LevelContext levelContext) : BaseLevel(levelContext) {
    }

    void onInit() override {
        randomSpawner.spawn(150000);
        atomWorld.addObject(Player{inputHandler, camera, shooter, atomWorld}, Vector2::fromCartesian(100, 100));
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