#pragma once

#include "../../Scene/Scene.h"
#include "../../utils/Rand.h"
#include "EmptyObject.h"
#include "EmptyStick.h"

enum class DrawingMode {
    CREATING,
    ERASING,
    BRIDGE,
    NONE,
};

class Drawing {
private:
    Scene &m_scene;
    InputBus &m_inputBus;
    IBHandle m_keyPressHandle;
    RNGf &m_gen;
    IBHandle m_keyReleaseHandle;
    IBHandle m_mousePressHandle;
    IBHandle m_mouseReleaseHandle;
    bool m_isActive = false;
    DrawingMode m_drawingMode = DrawingMode::NONE;
    Vector2F m_bridgeStart;
    Vector2F m_bridgeEnd;
public:
    explicit Drawing(Scene &scene, InputBus& inputBus, RNGf& gen) : m_scene(scene), m_inputBus(inputBus), m_gen(gen) {
        m_keyPressHandle = m_inputBus.addEventListener(sf::Event::KeyPressed, [this](const sf::Event &event) {
            std::cout << "Key pressed" << std::endl;
            if (event.key.code == sf::Keyboard::T) {
                m_drawingMode = DrawingMode::CREATING;
            } else if (event.key.code == sf::Keyboard::Y) {
                m_drawingMode = DrawingMode::ERASING;
            } else if (event.key.code == sf::Keyboard::U) {
                m_drawingMode = DrawingMode::BRIDGE;
            } else if (event.key.code == sf::Keyboard::I) {
                m_drawingMode = DrawingMode::NONE;
            }
        });
        m_mousePressHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](const sf::Event &event) {

            if (event.mouseButton.button == sf::Mouse::Left) {
                if (m_drawingMode == DrawingMode::BRIDGE) {
                    m_bridgeStart = m_scene.getCamera().screenPosToWorldPos(
                            sf::Mouse::getPosition(m_scene.getCamera().getWindow()));
                }
                m_isActive = true;
            }
        });
        m_mouseReleaseHandle = m_inputBus.addEventListener(sf::Event::MouseButtonReleased, [this](const sf::Event &event) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (m_isActive && m_drawingMode == DrawingMode::BRIDGE) {
                    float bridgeDensity = 10;
                    m_bridgeEnd = m_scene.getCamera().screenPosToWorldPos(
                            sf::Mouse::getPosition(m_scene.getCamera().getWindow()));
                    spawnBridge(m_bridgeStart, m_bridgeEnd, 4, 4);
                }
                m_isActive = false;
            }
        });
    }

    void spawnBridge(Vector2F start, Vector2F end, float objectInterval, float initialStretch = 1) {
        float bridgeLength = (end - start).magnitude();
        float howManyIntervalsFitIntoBridge = bridgeLength / objectInterval;

        int pointsCount = static_cast<int>(howManyIntervalsFitIntoBridge);

        float bridgeStepLength = objectInterval;
        Vector2F step = (end - start) / howManyIntervalsFitIntoBridge;

        std::vector<int> ids;
        ids.resize(pointsCount);
        for (int i = 0; i < pointsCount; i++) {
            std::weak_ptr<BaseObject> ptr = m_scene.getObjectStorage().addObject(EmptyObject{}, start + step * i);
            ids[i] = ptr.lock()->getId();
            ptr.lock()->getBasicDetails().m_color = sf::Color::Green;
            if (i == 0 || i == pointsCount - 1) {
                ptr.lock()->getBasicDetails().m_isPinned = true;
            }
        }

        for (int i = 0; i < pointsCount - 1; i++) {
            std::weak_ptr<BaseStick> ptr = m_scene.getStickStorage().addStick(EmptyStick{}, ids[i], ids[i + 1],
                                                                              bridgeStepLength / initialStretch);
            ptr.lock()->getBasicStickDetails().m_maxStretch = 100;
        }
    }

    void tick() {
        if (m_isActive) {
            Vector2F mousePos = m_scene.getCamera().screenPosToWorldPos(
                    sf::Mouse::getPosition(m_scene.getCamera().getWindow()));
            if (m_drawingMode == DrawingMode::CREATING) {
                std::cout << "Creating" << std::endl;
                int num = 25;
                float circleRadius = 100;
                for (int i = 0; i < num; i++) {
                    float leftMost = mousePos.m_x - circleRadius;
                    float rightMost = mousePos.m_x + circleRadius;
                    float topMost = mousePos.m_y - circleRadius;
                    float bottomMost = mousePos.m_y + circleRadius;

                    float x = m_gen.getInRange(leftMost, rightMost);
                    float y = m_gen.getInRange(topMost, bottomMost);
                    m_scene.getObjectStorage().addObject(EmptyObject{}, Vector2F::cart(x, y));
                }
            } else if (m_drawingMode == DrawingMode::ERASING) {
                float circleRadius = 100;
                m_scene.forEachInRadius(mousePos, circleRadius, [&](BaseObject *object, int i) {
                    // if inside - destroy. check by calculating difference magnitude
                    object->destroy();
                });
            } else if (m_drawingMode == DrawingMode::BRIDGE) {

            }
        }
    }
};