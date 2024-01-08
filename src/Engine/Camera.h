#pragma once


class Camera {
public:
    Vector2F position = Vector2F::cart(0, 0); // position itself (storing left top corner would make rotation very hard)
    Vector2F leftTopCorner = Vector2F::cart(0, 0); // position of the left top corner of the camera (just for to not calculate it every time)
    float baseViewWidth = 100;
    float baseViewHeight = 100;
    float zoomFactor = 1;

    void updateLeftTopCorner() {
        leftTopCorner = Vector2F::cart(getX1(), getY1());
    }

    void move(Vector2F delta) {
        position += delta;
        updateLeftTopCorner();
    }

    void setPosition(Vector2F vector) {
        position = vector;
        updateLeftTopCorner();
    }

    void zoom(float value) {
        zoomFactor *= value;
        zoomFactor = std::max(0.5f, zoomFactor);
        zoomFactor = std::min(64.0f, zoomFactor);
        updateLeftTopCorner();
    }

    [[nodiscard]] float getViewWidth() const {
        return baseViewWidth / zoomFactor;
    }

    [[nodiscard]] float getViewHeight() const {
        return baseViewHeight / zoomFactor;
    }

    [[nodiscard]] float getX1() const {
        return position.x - (baseViewWidth / zoomFactor) / 2;
    }
    [[nodiscard]] float getX2() const {
        return position.x + (baseViewWidth / zoomFactor) / 2;
    }
    [[nodiscard]] float getY1() const {
        return position.y - (baseViewHeight / zoomFactor) / 2;
    }
    [[nodiscard]] float getY2() const {
        return position.y + (baseViewHeight / zoomFactor) / 2;
    }

    [[nodiscard]] Vector2F getPosition() const {
        return position;
    }

    [[nodiscard]] Vector2F worldPosToScreenPos(Vector2F worldPosition) const {
        return (worldPosition - leftTopCorner) * zoomFactor;
    }

    [[nodiscard]] Vector2F screenPosToWorldPos(Vector2F screenPosition) const {
        return screenPosition / zoomFactor + leftTopCorner;
    }

    [[nodiscard]] float worldSizeToScreenSize(float worldSize) const {
        return worldSize * zoomFactor;
    }

    [[nodiscard]] float screenSizeToWorldSize(float screenSize) const {
        return screenSize / zoomFactor;
    }

    [[nodiscard]] Vector2F worldVectorToScreenVector(Vector2F worldVector) const {
        return worldVector * zoomFactor;
    }

    [[nodiscard]] Vector2F screenVectorToWorldVector(Vector2F screenVector) const {
        return screenVector / zoomFactor;
    }

    explicit Camera(float viewWidth, float viewHeight, Vector2F position) : baseViewWidth(viewWidth), baseViewHeight(viewHeight),
                                                                           position(position) {
        updateLeftTopCorner();
    };
};