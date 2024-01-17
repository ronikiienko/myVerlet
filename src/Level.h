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
    explicit Level(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000,2000), 600) {
    }

    void v_onInit() override {
        m_randomSpawner.spawn(150000);
        m_scene.addObject(Player{m_inputBus, m_scene}, Vector2F::cart(100, 100));
        std::cout << "Objects num: " << m_scene.getObjectsCount() << std::endl;

        m_inputBus.addEventListener(sf::Event::Resized, [&](const sf::Event &event) {
            sf::View view = sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width),
                                                   static_cast<float>(event.size.height)));
            m_window.setView(view);
        });
    }
    void v_onTick() override {}
};