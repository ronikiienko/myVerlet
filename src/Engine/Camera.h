#pragma once


#include "InputBus/InputBus.h"

class Camera {
private:
    InputBus &m_inputBus;
    sf::RenderWindow &m_window;
    float m_minZoom = 0.1;
    float m_zoom = 1;
    float m_maxZoom = 10;
    IBHandle windowResizeHandle;
public:
    void move(Vector2F delta) {
        sf::View view = m_window.getView();
        view.move(sf::Vector2f{delta.m_x, delta.m_y});
        m_window.setView(view);
    }

    void setPosition(Vector2F vector) {
        sf::View view = m_window.getView();
        view.setCenter(sf::Vector2f{vector.m_x, vector.m_y});
        m_window.setView(view);
    }

    Vector2F getPosition() {
        sf::View view = m_window.getView();
        return Vector2F::cart(view.getCenter().x, view.getCenter().y);
    }

    void zoom(float zoomFactor) {
        if (m_zoom * zoomFactor < m_minZoom) {
            zoomFactor = m_minZoom / m_zoom;
        } else if (m_zoom * zoomFactor > m_maxZoom) {
            zoomFactor = m_maxZoom / m_zoom;
        }

        m_zoom *= zoomFactor;
        sf::View view = m_window.getView();
        view.zoom(zoomFactor);
        m_window.setView(view);
    }

    void setZoom(float targetZoom) {
        float zoomFactor = targetZoom / m_zoom;
        zoom(zoomFactor);
    }

    sf::RenderWindow &getWindow() {
        return m_window;
    }

    void setMinZoom(float minZoom) {
        m_minZoom = minZoom;
    }
    void setMaxZoom(float maxZoom) {
        m_maxZoom = maxZoom;
    }
    void resetZoom() {
        float zoomFactor = 1 / m_zoom;
        m_zoom *= zoomFactor;
        sf::View view = m_window.getView();
        view.zoom(zoomFactor);
        m_window.setView(view);
    }

    void rotate(Angle angle) {
        sf::View view = m_window.getView();
        view.rotate(angle.getDegrees());
        m_window.setView(view);
    }

    [[nodiscard]] Vector2F worldPosToScreenPos(Vector2F worldPos) const {
        sf::Vector2i screenPos = m_window.mapCoordsToPixel(sf::Vector2f{worldPos.m_x, worldPos.m_y});
        return Vector2F::cart(static_cast<float>(screenPos.x) , static_cast<float>(screenPos.y));
    }

    [[nodiscard]] Vector2F screenPosToWorldPos(Vector2F screenPosition) const {
        sf::Vector2f worldPos = m_window.mapPixelToCoords({static_cast<int>(screenPosition.m_x), static_cast<int>(screenPosition.m_y)});
        return Vector2F::cart(worldPos.x, worldPos.y);
    }
    [[nodiscard]] Vector2F screenPosToWorldPos(sf::Vector2i screenPosition) const {
        sf::Vector2f worldPos = m_window.mapPixelToCoords({screenPosition.x, screenPosition.y});
        return Vector2F::cart(worldPos.x, worldPos.y);
    }

    explicit Camera(float maxWorldViewSize, Vector2F position, sf::RenderWindow &window, InputBus &inputBus) :
            m_inputBus(inputBus),
            m_window(window) {
        sf::View view = window.getView();
        view.setCenter(sf::Vector2f{position.m_x, position.m_y});

        float aspectRatio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);

        float newWorldViewWidth;
        float newWorldViewHeight;
        if (aspectRatio > 1) {
            newWorldViewWidth = maxWorldViewSize;
            newWorldViewHeight = maxWorldViewSize / aspectRatio;
        } else {
            newWorldViewWidth = maxWorldViewSize * aspectRatio;
            newWorldViewHeight = maxWorldViewSize;
        }
        view.setSize({newWorldViewWidth, newWorldViewHeight});
        window.setView(view);
        // when camera is moved, this is invalidated
        windowResizeHandle = m_inputBus.addEventListener(sf::Event::Resized, [this](const sf::Event &event) {
            float aspectRatio = static_cast<float>(m_window.getSize().x) / static_cast<float>(m_window.getSize().y);

            sf::View view = m_window.getView();

            float maxViewSize = std::max(view.getSize().x, view.getSize().y);

            float newWorldViewWidth;
            float newWorldViewHeight;
            if (aspectRatio > 1) {
                newWorldViewWidth = maxViewSize;
                newWorldViewHeight = maxViewSize / aspectRatio;
            } else {
                newWorldViewWidth = maxViewSize * aspectRatio;
                newWorldViewHeight = maxViewSize;
            }
            view.setSize({newWorldViewWidth, newWorldViewHeight});
            m_window.setView(view);
        });
    };

    Camera(const Camera &) = delete;

    Camera &operator=(const Camera &) = delete;

    Camera(Camera &&) = delete;

    Camera &operator=(Camera &&) = delete;
};