#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../Entities/Player.h"
#include "../consts.h"
#include "../Entities/Enemy.h"
#include "../../Engine/Modules/ExplosionHandler.h"
#include "../../Engine/Modules/RandomPositionGenerator.h"
#include "../Entities/Boid.h"
#include "CameraControls.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    SMHandle m_firstSound;
    IBHandle m_inputHandle;
    BoidsBlackboard m_blackboard;
    CameraControls m_move{m_scene, m_inputBus};
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(1000, 1000), 600) {
    }

    void v_onInit() override {
        m_inputHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](const sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                m_explosionHandler.launch(m_scene.getCamera().screenPosToWorldPos(Vector2F::cart(event.mouseButton.x, event.mouseButton.y)), 10, 100);
            }
        });
        m_scene.setCollisionsEnabled(false);
        m_physics.setSubSteps(8);
        m_gui.loadWidgetsFromFile("C:\\Libraries\\TGUI-1.1\\gui-builder\\boids.txt");
        m_gui.get<tgui::Slider>("separationSlider")->onValueChange([this](float value) {
            m_blackboard.m_separationWeight = value;
        });
        m_gui.get<tgui::Slider>("alignmentSlider")->onValueChange([this](float value) {
            m_blackboard.m_alignWeight = value;
        });
        m_gui.get<tgui::Slider>("cohesionSlider")->onValueChange([this](float value) {
            m_blackboard.m_cohesionWeight = value;
        });
        m_gui.get<tgui::Slider>("separationRadius")->onValueChange([this](float value) {
            m_blackboard.m_separationRadius = value * 100;
        });
        m_gui.get<tgui::Slider>("alignmentRadius")->onValueChange([this](float value) {
            m_blackboard.m_alignRadius = value * 100;
        });
        m_gui.get<tgui::Slider>("cohesionRadius")->onValueChange([this](float value) {
            m_blackboard.m_cohesionRadius = value * 100;
        });
        for (int i = 0; i < 1000; i++) {
            m_scene.addObject(Boid{m_scene.getObjectContext(), m_gen, m_blackboard}, m_randomPositionGenerator.get());
        }
        m_scene.forEachObject([&](BaseObject &obj, int id) {
            obj.getBasicDetails().accelerate(Vector2F::cart(m_gen.getInRange(-1, 1), m_gen.getInRange(-1, 1)) * 1000.0f);
        });
        auto playerPtr = m_scene.addObject(Player{m_scene.getObjectContext(), m_inputBus, m_window, m_gen},
                                           Vector2F::cart(102, 102));
//        for (int i = 0; i < 1000; i++) {
//            m_scene.addObject(Enemy{m_scene.getObjectContext(), dynamic_cast<Player*>(playerPtr.lock().get()), m_gen}, Vector2F::cart(m_gen.getInRange(0, 2000), m_gen.getInRange(0, 2000)));
//        }
        m_firstSound = m_soundManager.play("./res/ride.wav");
    }

    void v_onTick() override {}
};