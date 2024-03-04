#pragma once

#include "../../Engine/Scene.h"

class Move {
private:
    Scene &m_scene;
    InputBus &m_inputBus;
    IBHandle m_mouseClickHandle;
    IBHandle m_mouseReleaseHandle;
    IBHandle m_mouseMoveHandle;
    bool m_isMoving = false;
    sf::Window& m_window;
    Vector2F m_oldMousePos = Vector2F::cart(0, 0);
public:
    explicit Move(Scene &scene, InputBus &inputBus, sf::Window& window) : m_scene(scene), m_inputBus(inputBus), m_window(window) {
        m_mouseClickHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_isMoving = true;
                m_oldMousePos = Vector2F::cart(event.mouseButton.x, event.mouseButton.y);
            }
        });
        m_mouseReleaseHandle = m_inputBus.addEventListener(sf::Event::MouseButtonReleased, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_isMoving = false;
                m_oldMousePos = Vector2F::cart(0, 0);
            }
        });
        m_mouseMoveHandle = m_inputBus.addEventListener(sf::Event::MouseMoved, [this](sf::Event event) {
            std::cout << "Mouse moved" << std::endl;
            if (!m_isMoving) return;
            Vector2F newMousePos = Vector2F::cart(event.mouseMove.x, event.mouseMove.y);
            Vector2F delta = newMousePos - m_oldMousePos;
            Vector2F worldDelta = m_scene.getCamera().screenVectorToWorld(delta * -1);
            m_scene.getCamera().move(worldDelta);
            m_oldMousePos = newMousePos;
        });
    }
};