#pragma once


#include "InputHandler.h"

class Camera {
private:
    [[nodiscard]] float getWorldX1() const {
        return worldPosition.x - (baseWorldViewWidth / zoomFactor) / 2;
    }

    [[nodiscard]] float getWorldY1() const {
        return worldPosition.y - (baseWorldViewHeight / zoomFactor) / 2;
    }

    float biggestDimWorldViewSize = 100;
    float windowToCameraZoom;
public:
    Vector2F worldPosition = Vector2F::cart(0, 0); // worldPosition itself (storing left top corner would make rotation very hard)
    Vector2F worldLeftTopCorner = Vector2F::cart(0,
                                                 0); // worldPosition of the left top corner of the camera (just for to not calculate it every time)
    float baseWorldViewWidth;
    float baseWorldViewHeight;
    float zoomFactor = 1;
    InputHandler &inputHandler;
    sf::RenderWindow &window;

    void updateWorldLeftTopCorner() {
        worldLeftTopCorner = Vector2F::cart(getWorldX1(), getWorldY1());
    }

    void move(Vector2F delta) {
        worldPosition += delta;
        updateWorldLeftTopCorner();
    }

    void setPosition(Vector2F vector) {
        worldPosition = vector;
        updateWorldLeftTopCorner();
    }

    void zoom(float value) {
        zoomFactor *= value;
        zoomFactor = std::max(0.5f, zoomFactor);
        zoomFactor = std::min(64.0f, zoomFactor);
        updateWorldLeftTopCorner();
    }

    [[nodiscard]] float getViewWidth() const {
        return baseWorldViewWidth / getFinalZoom();
    }

    [[nodiscard]] float getViewHeight() const {
        return baseWorldViewHeight / getFinalZoom();
    }



    [[nodiscard]] Vector2F getPosition() const {
        return worldPosition;
    }

    [[nodiscard]] Vector2F worldPosToScreenPos(Vector2F worldPos) const {
        return (worldPos - worldLeftTopCorner) * getFinalZoom();
    }

    [[nodiscard]] Vector2F screenPosToWorldPos(Vector2F screenPosition) const {
        return screenPosition / getFinalZoom() + worldLeftTopCorner;
    }

    [[nodiscard]] float worldSizeToScreenSize(float worldSize) const {
        return worldSize * getFinalZoom();
    }

    [[nodiscard]] float screenSizeToWorldSize(float screenSize) const {
        return screenSize / getFinalZoom();
    }

    [[nodiscard]] Vector2F worldVectorToScreenVector(Vector2F worldVector) const {
        return worldVector * getFinalZoom();
    }

    [[nodiscard]] Vector2F screenVectorToWorldVector(Vector2F screenVector) const {
        return screenVector / getFinalZoom();
    }

    [[nodiscard]] float getFinalZoom() const {
        return zoomFactor * windowToCameraZoom;
    }

    explicit Camera(float biggestDimWorldViewSize, Vector2F position, sf::RenderWindow& window, InputHandler& inputHandler) : worldPosition(position), inputHandler(inputHandler), window(window), biggestDimWorldViewSize(biggestDimWorldViewSize) {
        float aspectRatio = static_cast<float>(window.getSize().x) / static_cast<float>(window.getSize().y);

        float biggestDimWindowSize = std::max(static_cast<float>(window.getSize().x) , static_cast<float>(window.getSize().y));
        windowToCameraZoom = biggestDimWindowSize / biggestDimWorldViewSize;

        if (aspectRatio > 1) {
            baseWorldViewWidth = biggestDimWorldViewSize;
            baseWorldViewHeight = biggestDimWorldViewSize / aspectRatio;
        } else {
            baseWorldViewWidth = biggestDimWorldViewSize * aspectRatio;
            baseWorldViewHeight = biggestDimWorldViewSize;
        }

        updateWorldLeftTopCorner();
    };
};