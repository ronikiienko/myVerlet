#pragma once

#include "../../Engine/BaseLevel.h"
#include "../../Engine/utils/Rand.h"
#include "../consts.h"
#include "../../Engine/Modules/ExplosionHandler.h"
#include "../../Engine/Modules/RandomPositionGenerator.h"
#include "../../Engine/Modules/CameraControls.h"
#include "../../Engine/Modules/PerformanceTest/EmptyObject.h"
#include "../Entities/Player.h"
#include "../Entities/Enemy.h"
#include "../UiRenderer.h"
#include "../Entities/Food.h"

class Level1 : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{consts::seed};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    SMHandle m_firstSound;
    IBHandle m_inputHandle;
    CameraControls m_move{m_scene, m_inputBus};
    UiRenderer uiRenderer{m_eventBus, m_gui};

    IBHandle m_keyPressHandle;
public:
    explicit Level1(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(2000, 2000), 600) {
    }

    void v_onInit() override {
        Shooter playerShooter{m_scene, m_gen};
        CameraFollowing cameraFollowing{m_scene.getCamera()};
        std::weak_ptr<BaseObject> playerGeneralPtr = m_scene.getObjectStorage().addObject(Player{m_inputBus, m_gen, m_eventBus, playerShooter, cameraFollowing}, Vector2F::cart(100, 100));
        std::weak_ptr<Player> playerPtr = std::static_pointer_cast<Player>(playerGeneralPtr.lock());
        for (int i = 0; i < 100; i++) {
            m_scene.getObjectStorage().addObject(Enemy{playerPtr, m_gen, m_eventBus}, m_randomPositionGenerator.get());
        }
        for (int i = 0; i < 0; i++) {
            m_scene.getObjectStorage().addObject(EmptyObject{}, m_randomPositionGenerator.get());
        }
        for (int i = 0; i < 100; i++) {
            m_scene.getObjectStorage().addObject(Food{}, m_randomPositionGenerator.get());
        }

        m_keyPressHandle = m_inputBus.addEventListener(sf::Event::KeyPressed,[this](sf::Event event) {
            if (event.key.code == sf::Keyboard::O) {
                m_scene.getObjectStorage().logDifferentTypeCounts();
            }
            if (event.key.code == sf::Keyboard::R) {
                m_scene.getObjectStorage().forEachObjectOfType<Enemy>([this](Enemy& enemy, int id){
                    enemy.destroy();
                });
            }
            if (event.key.code == sf::Keyboard::P) {
                m_scene.getObjectStorage().forEachObjectOfType<Bullet>([this](Bullet& bullet, int id){
                    bullet.destroy();
                });
            }
        });
    }

    void v_onTick() override {
        m_eventBus.emit(GameEvents::EnemyCountUpdate{m_scene.getObjectStorage().getObjectsOfTypeCount<Enemy>()});
    }
};