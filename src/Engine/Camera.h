#pragma once


#include "InputHandler.h"

class Camera {
private:
    [[nodiscard]] float getWorldX1() const {
        return m_worldPosition.m_x - (m_baseWorldViewWidth / m_zoomFactor) / 2;
    }

    [[nodiscard]] float getWorldY1() const {
        return m_worldPosition.m_y - (m_baseWorldViewHeight / m_zoomFactor) / 2;
    }

    void updateWorldLeftTopCorner() {
        m_worldLeftTopCorner = Vector2F::cart(getWorldX1(), getWorldY1());
    }

    [[nodiscard]] float getFinalZoom() const {
        return m_zoomFactor * m_windowToCameraZoom;
    }

    float m_maxWorldViewSize = 100;

    // m_worldPosition itself (storing left top corner would make rotation very hard)
    Vector2F m_worldPosition = Vector2F::cart(0, 0);
    // m_worldPosition of the left top corner of the m_camera (just for to not calculate it every time)
    Vector2F m_worldLeftTopCorner = Vector2F::cart(0, 0);

    float m_baseWorldViewWidth;
    float m_baseWorldViewHeight;

    float m_windowToCameraZoom;
    float m_zoomFactor = 1;

    InputHandler &m_inputHandler;
    sf::RenderWindow &m_window;
public:
    void move(Vector2F delta) {
        m_worldPosition += delta;
        updateWorldLeftTopCorner();
    }

    void setPosition(Vector2F vector) {
        m_worldPosition = vector;
        updateWorldLeftTopCorner();
    }

    void zoom(float value) {
        m_zoomFactor *= value;
        m_zoomFactor = std::max(0.5f, m_zoomFactor);
        m_zoomFactor = std::min(64.0f, m_zoomFactor);
        updateWorldLeftTopCorner();
    }

    [[nodiscard]] Vector2F getPosition() const {
        return m_worldPosition;
    }

    [[nodiscard]] Vector2F worldPosToScreenPos(Vector2F worldPos) const {
        return (worldPos - m_worldLeftTopCorner) * getFinalZoom();
    }

    [[nodiscard]] Vector2F screenPosToWorldPos(Vector2F screenPosition) const {
        return screenPosition / getFinalZoom() + m_worldLeftTopCorner;
    }

    [[nodiscard]] float worldSizeToScreenSize(float worldSize) const {
        return worldSize * getFinalZoom();
    }

    [[nodiscard]] float screenSizeToWorldSize(float screenSize) const {
        return screenSize / getFinalZoom();
    }


    explicit Camera(float maxWorldViewSize, Vector2F position, sf::RenderWindow &window, InputHandler &inputHandler) :
            m_worldPosition(position),
            m_inputHandler(inputHandler),
            m_window(window),
            m_maxWorldViewSize(maxWorldViewSize) {
        float aspectRatio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);

        float biggestDimWindowSize = std::max(static_cast<float>(window.getSize().x),
                                              static_cast<float>(window.getSize().y));
        m_windowToCameraZoom = biggestDimWindowSize / maxWorldViewSize;

        if (aspectRatio > 1) {
            m_baseWorldViewWidth = maxWorldViewSize;
            m_baseWorldViewHeight = maxWorldViewSize / aspectRatio;
        } else {
            m_baseWorldViewWidth = maxWorldViewSize * aspectRatio;
            m_baseWorldViewHeight = maxWorldViewSize;
        }

        updateWorldLeftTopCorner();
    };
};