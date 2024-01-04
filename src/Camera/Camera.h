#pragma once

#include "../utils/Vector.h"

class Camera {
public:
    Vector2 position = Vector2::fromCartesian(0,0);
    Vector2 leftTopCorner = Vector2::fromCartesian(0,0);
    float viewWidth = 100;
    float viewHeight = 100;
    float zoomFactor = 1;

    void updateLeftTopCorner() {
        leftTopCorner = Vector2::fromCartesian(getX1(), getY1());
    }

    void move(Vector2 delta) {
        position += delta;
        updateLeftTopCorner();
    }

    void setPosition(Vector2 vector) {
        position = vector;
        updateLeftTopCorner();
    }

    void zoom(float value) {
        zoomFactor *= value;
        zoomFactor = std::max(0.5f, zoomFactor);
        zoomFactor = std::min(64.0f, zoomFactor);
        updateLeftTopCorner();
    }

    [[nodiscard]] float getX1() const {
        return position.x - (viewWidth / zoomFactor) / 2;
    }
    [[nodiscard]] float getX2() const {
        return position.x + (viewWidth / zoomFactor) / 2;
    }
    [[nodiscard]] float getY1() const {
        return position.y - (viewHeight / zoomFactor) / 2;
    }
    [[nodiscard]] float getY2() const {
        return position.y + (viewHeight / zoomFactor) / 2;
    }

    [[nodiscard]] Vector2 getPosition() const {
        return position;
    }

    [[nodiscard]] Vector2 worldPosToScreenPos(Vector2 worldPosition) const {
        return (worldPosition - leftTopCorner) * zoomFactor;
    }

    [[nodiscard]] Vector2 screenPosToWorldPos(Vector2 screenPosition) const {
        return screenPosition / zoomFactor + leftTopCorner;
    }

    [[nodiscard]] float worldSizeToScreenSize(float worldSize) const {
        return worldSize * zoomFactor;
    }

    [[nodiscard]] float screenSizeToWorldSize(float screenSize) const {
        return screenSize / zoomFactor;
    }

    explicit Camera(float viewWidth, float viewHeight, Vector2 position) : viewWidth(viewWidth), viewHeight(viewHeight),
                                                                           position(position) {
        updateLeftTopCorner();
    };
};