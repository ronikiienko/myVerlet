#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../Entities/Player.h"
#include "../consts.h"
#include "../Entities/Enemy.h"
#include "../../Engine/Modules/ExplosionHandler.h"
#include "../../Engine/Modules/RandomPositionGenerator.h"
#include "../Entities/Boid.h"
#include "../Entities/Predator.h"
#include "../../Engine/Modules/CameraControls.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    SMHandle m_firstSound;
    IBHandle m_inputHandle;
    BoidsBlackboard m_blackboard;
    CameraControls m_move{m_scene, m_inputBus};
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        m_scene.setCollisionsEnabled(true);
        m_physics.setLinearDamping(1);
        m_physics.setSubSteps(1);
        m_scene.setBoundaryType(BOUNDARY_TYPE::WRAP);
        m_physics.setWallsDamping(1);
        m_inputHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](const sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                m_explosionHandler.launch(m_scene.getCamera().screenPosToWorldPos(Vector2F::cart(event.mouseButton.x, event.mouseButton.y)), 2, 200);
            }
        });

        m_gui.loadWidgetsFromFile("C:\\Libraries\\TGUI-1.1\\gui-builder\\boids.txt");

        tgui::Slider::Ptr separationSlider = m_gui.get<tgui::Slider>("separationSlider");
        tgui::Slider::Ptr alignmentSlider = m_gui.get<tgui::Slider>("alignmentSlider");
        tgui::Slider::Ptr cohesionSlider = m_gui.get<tgui::Slider>("cohesionSlider");

        m_physics.setSubSteps(8);
        separationSlider->onValueChange([this](float value) {
            m_blackboard.m_separationWeight = value;
            std::cout << "Separation weight: " << value << std::endl;
        });
        alignmentSlider->onValueChange([this](float value) {
            m_blackboard.m_alignWeight = value;
            std::cout << "Alignment weight: " << value << std::endl;
        });
        cohesionSlider->onValueChange([this](float value) {
            m_blackboard.m_cohesionWeight = value;
            std::cout << "Cohesion weight: " << value << std::endl;
        });
        m_gui.get<tgui::Button>("randomize")->onPress([this](){
            m_blackboard.m_alignWeight = m_gen.getInRange(0, 1);
            m_blackboard.m_cohesionWeight = m_gen.getInRange(0, 1);
            m_blackboard.m_separationWeight = m_gen.getInRange(0, 1);

            m_gui.get<tgui::Slider>("separationSlider")->setValue(m_blackboard.m_separationWeight);
            m_gui.get<tgui::Slider>("alignmentSlider")->setValue(m_blackboard.m_alignWeight);
            m_gui.get<tgui::Slider>("cohesionSlider")->setValue(m_blackboard.m_cohesionWeight);

        });
        m_gui.get<tgui::Button>("add100")->onPress([this](){
            for (int i = 0; i < 1000; i++) {
                m_scene.addObject(Boid{m_scene.getObjectContext(), m_gen, m_blackboard}, m_randomPositionGenerator.get());
            }
        });
        for (int i = 0; i < 1000; i++) {
            m_scene.addObject(Boid{m_scene.getObjectContext(), m_gen, m_blackboard}, m_randomPositionGenerator.get());
        }
        for (int i = 0; i < 100; i++) {
            m_scene.addObject(Predator{m_scene.getObjectContext(), m_gen, m_blackboard}, m_randomPositionGenerator.get());
        }
        auto playerPtr = m_scene.addObject(Player{m_scene.getObjectContext(), m_inputBus, m_window, m_gen},
                                           Vector2F::cart(102, 102));
//        for (int i = 0; i < 1000; i++) {
//            m_scene.addObject(Enemy{m_scene.getObjectContext(), dynamic_cast<Player*>(playerPtr.lock().get()), m_gen}, Vector2F::cart(m_gen.getInRange(0, 2000), m_gen.getInRange(0, 2000)));
//        }
        m_firstSound = m_soundManager.play("./res/ride.wav");
    }

    void v_onTick() override {
        m_gui.get<tgui::Label>("objectCount")->setText("Object count: " + std::to_string(m_scene.getObjectsCount()));
    }
};