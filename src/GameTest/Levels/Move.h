#pragma once

#include "../../Engine/Scene.h"

class Move {
private:
    Scene &m_scene;
    InputBus &m_inputBus;
    IBHandle m_mouseClickHandle;
    IBHandle m_mouseReleaseHandle;
    IBHandle m_mouseMoveHandle;
    IBHandle m_mouseWheelHandle;
    IBHandle m_keyPressHandle;
    bool m_isMoving = false;
    sf::RenderWindow& m_window;
    sf::Vector2i m_oldMouseScreenPos{0, 0};
    float m_arrowMoveSpeed = 50;
public:
    explicit Move(Scene &scene, InputBus &inputBus, sf::RenderWindow& window) : m_scene(scene), m_inputBus(inputBus), m_window(window) {
        m_mouseClickHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_isMoving = true;
                m_oldMouseScreenPos = {event.mouseButton.x, event.mouseButton.y};
            }
        });
        m_mouseReleaseHandle = m_inputBus.addEventListener(sf::Event::MouseButtonReleased, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_isMoving = false;
                m_oldMouseScreenPos = {0, 0};
            }
        });
        m_mouseMoveHandle = m_inputBus.addEventListener(sf::Event::MouseMoved, [this](sf::Event event) {
            if (!m_isMoving) return;
            Camera &camera = m_scene.getCamera();

            sf::Vector2i newMouseScreenPos = {event.mouseMove.x, event.mouseMove.y};

            Vector2F worldDelta = camera.screenPosToWorldPos(newMouseScreenPos) - camera.screenPosToWorldPos(m_oldMouseScreenPos);
            camera.move(-worldDelta);

            m_oldMouseScreenPos = newMouseScreenPos;
        });
        m_mouseWheelHandle = m_inputBus.addEventListener(sf::Event::MouseWheelScrolled, [this](sf::Event event) {
            if (event.mouseWheelScroll.delta > 0) {
                m_scene.getCamera().zoom(0.75);
            } else {
                m_scene.getCamera().zoom(1.25);
            }
        });
        m_keyPressHandle = m_inputBus.addEventListener(sf::Event::KeyPressed, [this](sf::Event event) {
            switch (event.key.code) {
                case sf::Keyboard::Key::Right: {
                    m_scene.getCamera().move(Vector2F::cart(m_arrowMoveSpeed, 0));
                    break;
                }

                case sf::Keyboard::Key::Left: {
                    m_scene.getCamera().move(Vector2F::cart(-m_arrowMoveSpeed, 0));
                    break;
                }

                case sf::Keyboard::Key::Up: {
                    m_scene.getCamera().move(Vector2F::cart(0, -m_arrowMoveSpeed));
                    break;
                }

                case sf::Keyboard::Key::Down: {
                    m_scene.getCamera().move(Vector2F::cart(0, m_arrowMoveSpeed));
                    break;
                }
                default:
                    break;
            }
        });
    }
};