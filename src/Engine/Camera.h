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

    void updateWorldLeftTopCorner() {
        worldLeftTopCorner = Vector2F::cart(getWorldX1(), getWorldY1());
    }

    [[nodiscard]] float getFinalZoom() const {
        return zoomFactor * windowToCameraZoom;
    }

    float biggestDimWorldViewSize = 100;

    // worldPosition itself (storing left top corner would make rotation very hard)
    Vector2F worldPosition = Vector2F::cart(0, 0);
    // worldPosition of the left top corner of the camera (just for to not calculate it every time)
    Vector2F worldLeftTopCorner = Vector2F::cart(0,0);

    float baseWorldViewWidth;
    float baseWorldViewHeight;

    float windowToCameraZoom;
    float zoomFactor = 1;

    InputHandler &inputHandler;
    sf::RenderWindow &window;
public:
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