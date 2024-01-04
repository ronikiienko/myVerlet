#pragma once

#include "../utils/Vector.h"

class Camera {
public:
    Vector2 position = Vector2::fromCartesian(10000,0);
    float viewWidth = 100;
    float viewHeight = 100;
    float zoomFactor = 1;

    void move(Vector2 delta) {
        position += delta;
    }

    void setPosition(Vector2 vector) {
        position = vector;
    }

    void zoom(float value) {
        zoomFactor *= value;
        zoomFactor = std::max(1.0f, zoomFactor);
        zoomFactor = std::min(64.0f, zoomFactor);
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

    explicit Camera(float viewWidth, float viewHeight) : viewWidth(viewWidth), viewHeight(viewHeight) {};
};