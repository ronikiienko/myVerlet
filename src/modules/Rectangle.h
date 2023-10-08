#pragma once

class Rectangle {
private:
    float x1, x2, y1, y2;

    Rectangle(float x1, float y1, float x2, float y2) : x1(x1), x2(x2), y1(y1), y2(y2) {};
public:

    static Rectangle fromCoords(float x1, float y1, float x2, float y2) {
        return {x1, y1, x2, y2};
    }

    static Rectangle fromSize(float x1, float y1, float width, float height) {
        return {x1, y1, x1 + width, y1 + height};
    }

    [[nodiscard]] float getX1() const {
        return x1;
    }

    [[nodiscard]] float getX2() const {
        return x2;
    }

    [[nodiscard]] float getY1() const {
        return y1;
    }

    [[nodiscard]] float getY2() const {
        return y2;
    }

    [[nodiscard]] float getWidth() const {
        return x2 - x1;
    }

    [[nodiscard]] float getHeight() const {
        return y2 - y1;
    }
};