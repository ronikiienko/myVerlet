#pragma once

template <typename T>
class Rectangle {
private:
    T x1, x2, y1, y2;

    Rectangle(T x1, T y1, T x2, T y2) : x1(x1), x2(x2), y1(y1), y2(y2) {};
public:

    static Rectangle fromCoords(T x1, T y1, T x2, T y2) {
        return {x1, y1, x2, y2};
    }

    static Rectangle fromSize(T x1, T y1, T width, T height) {
        return {x1, y1, x1 + width, y1 + height};
    }

    [[nodiscard]] T getX1() const {
        return x1;
    }

    [[nodiscard]] T getX2() const {
        return x2;
    }

    [[nodiscard]] T getY1() const {
        return y1;
    }

    [[nodiscard]] T getY2() const {
        return y2;
    }

    [[nodiscard]] T getWidth() const {
        return x2 - x1;
    }

    [[nodiscard]] T getHeight() const {
        return y2 - y1;
    }
};

using RectangleF = Rectangle<float>;
using RectangleI = Rectangle<int>;