#pragma once

#include "../Scene/Scene.h"

class CameraControls {
private:
    Scene &m_scene;
    InputBus &m_inputBus;
    IBHandle m_mouseClickHandle;
    IBHandle m_mouseReleaseHandle;
    IBHandle m_mouseMoveHandle;
    IBHandle m_mouseWheelScrolledHandle;
    bool m_isMoving = false;
    bool m_isRotating = false;
    sf::Vector2i m_oldMouseScreenPos{0, 0};
public:
    explicit CameraControls(Scene &scene, InputBus &inputBus) : m_scene(scene), m_inputBus(inputBus){
        m_scene.getCamera().setMinZoom(0.1);
        m_scene.getCamera().setMaxZoom(10);
        m_mouseClickHandle = m_inputBus.addEventListener(sf::Event::MouseButtonPressed, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_isMoving = true;
                m_oldMouseScreenPos = {event.mouseButton.x, event.mouseButton.y};
            }
            if (event.mouseButton.button == sf::Mouse::Middle) {
                m_isRotating = true;
            }
        });
        m_mouseReleaseHandle = m_inputBus.addEventListener(sf::Event::MouseButtonReleased, [this](sf::Event event) {
            if (event.mouseButton.button == sf::Mouse::Right) {
                m_isMoving = false;
                m_oldMouseScreenPos = {0, 0};
            }
            if (event.mouseButton.button == sf::Mouse::Middle) {
                m_isRotating = false;
            }
        });
        m_mouseMoveHandle = m_inputBus.addEventListener(sf::Event::MouseMoved, [this](sf::Event event) {
            sf::Vector2i newMouseScreenPos = {event.mouseMove.x, event.mouseMove.y};

            if (m_isMoving) {
                Camera &camera = m_scene.getCamera();
                Vector2F worldDelta =
                        camera.screenPosToWorldPos(newMouseScreenPos) - camera.screenPosToWorldPos(m_oldMouseScreenPos);
                camera.move(-worldDelta);
            }
            if (m_isRotating) {
                Camera &camera = m_scene.getCamera();
                sf::Vector2i screenDelta = newMouseScreenPos - m_oldMouseScreenPos;
                float maxRotation = 10;
                float coefficient = std::clamp(static_cast<float>(screenDelta.x) / 20, -1.0f, 1.0f);
                camera.rotate(Angle::fromDegrees(maxRotation * coefficient));
            }


            m_oldMouseScreenPos = newMouseScreenPos;
        });
        m_mouseWheelScrolledHandle = m_inputBus.addEventListener(sf::Event::MouseWheelScrolled,
                                                                 [this](sf::Event event) {
                                                                     if (event.mouseWheelScroll.delta > 0) {
                                                                         m_scene.getCamera().zoom(0.75);
                                                                     } else {
                                                                         m_scene.getCamera().zoom(1.25);
                                                                     }
                                                                 });
    }
};