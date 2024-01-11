#pragma once

#include "Engine/BaseLevel.h"
#include "Engine/utils/Rand.h"
#include "addons/RandomSpawner.h"
#include "Objects/Player.h"
#include "addons/Shooter.h"
#include "consts.h"

class Level : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    Shooter m_shooter{Vector2F::cart(200, 200), 2, 1, 4, m_scene, 1, m_explosionHandler};
    RNGf m_gen{consts::seed};
    RandomSpawner m_randomSpawner{m_scene, m_gen};
public:
    explicit Level(LevelContext levelContext) : BaseLevel(levelContext, 200000) {
    }

    void onInit() override {
        m_randomSpawner.spawn(1500);
        m_scene.addObject(Player{m_inputHandler, m_shooter, m_scene}, Vector2F::cart(100, 100));
        m_inputHandler.addEventListener(sf::Event::KeyPressed, [&](const sf::Event &event) {
            if (event.key.code == sf::Keyboard::BackSpace) {
                m_scene.clear();
            }

            if (event.key.code == sf::Keyboard::M) {
                m_randomSpawner.spawn(5000);
            }
        });
        m_inputHandler.addEventListener(sf::Event::MouseButtonPressed, [&](const sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_explosionHandler.launch(
                        m_scene.getCamera().screenPosToWorldPos(Vector2F::cart(static_cast<float>(event.mouseButton.x),
                                                                               static_cast<float>(event.mouseButton.y))),
                        4,
                        150);
            }
        });
        std::cout << "Objects num: " << m_scene.getObjectsCount() << std::endl;
    }

};