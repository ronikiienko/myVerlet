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

    void onInit() override {
        m_randomSpawner.spawn(150000);
        m_scene.addObject(Player{m_inputHandler, m_shooter, m_scene}, Vector2F::cart(100, 100));
        std::cout << "Objects num: " << m_scene.getObjectsCount() << std::endl;

//        auto label = tgui::Label::create(); // Create a label
//        label->setText("Hello, World!"); // Set the text
//        label->setTextSize(24); // Set the text size
//        label->setPosition("(&.width - width) / 2", "(&.height - height) / 2"); // Center the label
//
//        m_gui.add(label);
    }
    void onTick() override {}
};