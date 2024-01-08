#pragma once

#include "Engine/BaseLevel.h"
#include "Engine/utils/Rand.h"
#include "addons/RandomSpawner.h"
#include "Objects/Player.h"
#include "addons/Shooter.h"

class Level : public BaseLevel {
    ExplosionHandler explosionHandler{scene};
    Shooter shooter{Vector2F::cart(200, 200), 2, 1, 4, scene, 1, explosionHandler};
    RNGf gen{consts::seed};
    RandomSpawner randomSpawner{scene, gen};
public:
    explicit Level(LevelContext levelContext) : BaseLevel(levelContext) {
    }

    void onInit() override {
        randomSpawner.spawn(150000);
        scene.addObject(Player{inputHandler, shooter, scene}, Vector2F::cart(100, 100));
        inputHandler.addEventListener(sf::Event::KeyPressed, [&](sf::Event &event) {
            if (event.key.code == sf::Keyboard::BackSpace) {
                scene.clear();
            }

            if (event.key.code == sf::Keyboard::M) {
                randomSpawner.spawn(5000);
            }
        });
        inputHandler.addEventListener(sf::Event::MouseButtonPressed, [&](sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                explosionHandler.launch(
                        scene.getCamera().screenPosToWorldPos(Vector2F::cart(static_cast<float>(event.mouseButton.x),
                                                                 static_cast<float>(event.mouseButton.y))),
                        4,
                        150);
            }
        });
        std::cout << "Objects num: " << scene.getObjectsCount() << std::endl;
    }

};