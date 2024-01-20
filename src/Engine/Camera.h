#pragma once


#include "InputBus/InputBus.h"

class Camera {
private:
    [[nodiscard]] float getWorldX1() const {
        return m_worldCenterPos.m_x - (m_baseWorldViewWidth / m_zoomFactor) / 2;
    }

    [[nodiscard]] float getWorldY1() const {
        return m_worldCenterPos.m_y - (m_baseWorldViewHeight / m_zoomFactor) / 2;
    }

    void updateWorldLeftTopPos() {
        m_worldLeftTopPos = Vector2F::cart(getWorldX1(), getWorldY1());
    }

    [[nodiscard]] float getFinalZoom() const {
        return m_zoomFactor * m_windowToCameraZoom;
    }

    float m_maxWorldViewSize;

    Vector2F m_worldCenterPos = Vector2F::cart(0, 0);
    Vector2F m_worldLeftTopPos = Vector2F::cart(0, 0);

    float m_baseWorldViewWidth;
    float m_baseWorldViewHeight;

    float m_windowToCameraZoom;
    float m_zoomFactor = 1;

    InputBus &m_inputBus;
    sf::RenderWindow &m_window;

    IBHandle windowResizeHandle;
public:
    void move(Vector2F delta) {
        m_worldCenterPos += delta;
        updateWorldLeftTopPos();
    }

    void setPosition(Vector2F vector) {
        m_worldCenterPos = vector;
        updateWorldLeftTopPos();
    }

    void zoom(float zoomFactor) {
        m_zoomFactor *= zoomFactor;
        m_zoomFactor = std::max(0.1f, m_zoomFactor);
        m_zoomFactor = std::min(64.0f, m_zoomFactor);
        updateWorldLeftTopPos();
    }

    [[nodiscard]] Vector2F getPosition() const {
        return m_worldCenterPos;
    }

    [[nodiscard]] Vector2F worldPosToScreenPos(Vector2F worldPos) const {
        return (worldPos - m_worldLeftTopPos) * getFinalZoom();
    }

    [[nodiscard]] Vector2F screenPosToWorldPos(Vector2F screenPosition) const {
        return screenPosition / getFinalZoom() + m_worldLeftTopPos;
    }

    [[nodiscard]] float worldSizeToScreenSize(float worldSize) const {
        return worldSize * getFinalZoom();
    }

    [[nodiscard]] float screenSizeToWorldSize(float screenSize) const {
        return screenSize / getFinalZoom();
    }


    explicit Camera(float maxWorldViewSize, Vector2F position, sf::RenderWindow &window, InputBus &inputBus) :
            m_worldCenterPos(position),
            m_inputBus(inputBus),
            m_window(window),
            m_maxWorldViewSize(maxWorldViewSize) {
        float aspectRatio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);

        float biggestDimWindowSize = std::max(static_cast<float>(window.getSize().x),
                                              static_cast<float>(window.getSize().y));
        m_windowToCameraZoom = biggestDimWindowSize / maxWorldViewSize;

        if (aspectRatio > 1) {
            m_baseWorldViewWidth = m_maxWorldViewSize;
            m_baseWorldViewHeight = m_maxWorldViewSize / aspectRatio;
        } else {
            m_baseWorldViewWidth = m_maxWorldViewSize * aspectRatio;
            m_baseWorldViewHeight = m_maxWorldViewSize;
        }

        // when camera is moved, this is invalidated
        windowResizeHandle = m_inputBus.addEventListener(sf::Event::Resized, [this](const sf::Event &event) {
            float aspectRatio = static_cast<float>(event.size.width) / static_cast<float>(event.size.height);
            float biggestDimWindowSize = std::max(static_cast<float>(event.size.width),
                                                  static_cast<float>(event.size.height));
            m_windowToCameraZoom = biggestDimWindowSize / m_maxWorldViewSize;
            if (aspectRatio > 1) {
                m_baseWorldViewWidth = m_maxWorldViewSize;
                m_baseWorldViewHeight = m_maxWorldViewSize / aspectRatio;
            } else {
                m_baseWorldViewWidth = m_maxWorldViewSize * aspectRatio;
                m_baseWorldViewHeight = m_maxWorldViewSize;
            }
        });

        updateWorldLeftTopPos();
    };

    Camera(const Camera &) = delete;
    Camera& operator=(const Camera &) = delete;
    Camera(Camera &&) = delete;
    Camera& operator=(Camera &&) = delete;
};