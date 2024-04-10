//#pragma once
//
//#include "../../BaseLevel.h"
//#include "../../utils/Rand.h"
//#include "../ExplosionHandler.h"
//#include "EmptyObject.h"
//#include "../RandomPositionGenerator.h"
//#include "../CameraControls.h"
//#include "EmptyStick.h"
//
//class PerformanceTest : public BaseLevel {
//    ExplosionHandler m_explosionHandler{m_scene};
//    RNGf m_gen{10};
//    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
//    CameraControls m_cameraControls{m_scene, m_inputBus};
//    IBHandle m_mouseClickHandle;
//public:
//    explicit PerformanceTest(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(1000, 1000),
//                                                                    600) {
//    }
//
//    void v_onInit() override {
//        for (int i = 0; i < 150000; i++) {
//            m_scene.getObjectStorage().addObject(EmptyObject{}, m_randomPositionGenerator.get());
//        };
////        m_scene.getStickStorage().addStick(EmptyStick{}, 0, 1, 20);
////        m_scene.getStickStorage().addStick(EmptyStick{}, 1, 2, 20);
////        m_scene.getStickStorage().addStick(EmptyStick{}, 2, 0, 20);
////        for (int i = 1; i < 10000; i++) {
////            m_scene.getStickStorage().addStick(EmptyStick{}, i, i - 1, 20);
////        }
//        m_mouseClickHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](sf::Event event) {
//            Vector2F mousePos = m_scene.getCamera().screenPosToWorldPos(
//                    sf::Mouse::getPosition(m_scene.getCamera().getWindow()));
//            m_scene.getObjectStorage().forEachBasicDetails([&](BasicDetails &object, int i) {
//                // if inside - destroy. check by calculating difference magnitude
//                Vector2F dist = mousePos - object.m_posCurr;
//                float distMagnitude = dist.magnitude();
//
//                if (distMagnitude < engineDefaults::objectsRadius) {
//                    m_scene.getObjectStorage().removeObject(i);
//                }
//            });
//        });
//    }
//
//    void v_onTick() override {}
//};

#pragma once

#include "../../BaseLevel.h"
#include "../../utils/Rand.h"
#include "../ExplosionHandler.h"
#include "EmptyObject.h"
#include "../RandomPositionGenerator.h"
#include "../CameraControls.h"
#include "EmptyStick.h"
#include "Drawing.h"

class PerformanceTest : public BaseLevel {
    ExplosionHandler m_explosionHandler{m_scene};
    RNGf m_gen{10};
    RandomPositionGenerator m_randomPositionGenerator{m_scene, m_gen};
    CameraControls m_cameraControls{m_scene, m_inputBus};
    Drawing m_drawing{m_scene, m_inputBus, m_gen};
public:
    explicit PerformanceTest(LevelContext levelContext) : BaseLevel(levelContext, 200000, Vector2I::cart(1000, 1000),
                                                                    600) {
    }

    void v_onInit() override {
        m_scene.setBoundaryType(BOUNDARY_TYPE::REMOVE);
        m_scene.setGravity(Vector2F::cart(0, 0.01));
//         spawn many triangles
//        int numberOfTriangles = 10000;
//        for (int i = 0; i < numberOfTriangles * 3; i++) {
//            m_scene.getObjectStorage().addObject(EmptyObject{}, m_randomPositionGenerator.get());
//        }
//
//        for (int i = 0; i < numberOfTriangles; i++) {
//            m_scene.getStickStorage().addStick(EmptyStick{}, i * 3, i * 3 + 1, 20);
//            m_scene.getStickStorage().addStick(EmptyStick{}, i * 3 + 1, i * 3 + 2, 20);
//            m_scene.getStickStorage().addStick(EmptyStick{}, i * 3 + 2, i * 3, 20);
//        }

        // spawn cloth
//        int clothSize = 50;
//        for (int i = 0; i < clothSize; i++) {
//            for (int j = 0; j < clothSize; j++) {
//                m_scene.getObjectStorage().addObject(EmptyObject{}, Vector2F::cart(i * 20, j * 20));
//            }
//        }



//        m_scene.setCollisionsEnabled(false);
//        for (int i = 0; i < 60000; i++) {
//            m_scene.getObjectStorage().addObject(EmptyObject{}, m_randomPositionGenerator.get());
//        };
//        m_scene.getStickStorage().addStick(EmptyStick{}, 0, 1, 20);
//        m_scene.getStickStorage().addStick(EmptyStick{}, 1, 2, 20);
//        m_scene.getStickStorage().addStick(EmptyStick{}, 2, 0, 20);
//        for (int i = 1; i < 60000; i++) {
//            m_scene.getStickStorage().addStick(EmptyStick{}, i, i - 1, 10);
//        }
    }

    void v_onTick() override {
        m_drawing.tick();
    }
};