#pragma once

template <typename T>
class Rectangle {
private:
    T m_x1, m_x2, m_y1, m_y2;

    constexpr Rectangle(T x1, T y1, T x2, T y2) : m_x1(x1), m_x2(x2), m_y1(y1), m_y2(y2) {};
public:

    constexpr static Rectangle fromCoords(T x1, T y1, T x2, T y2) {
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        if (y1 > y2) {
            std::swap(y1, y2);
        }
        return {x1, y1, x2, y2};
    }

    constexpr static Rectangle fromCoords(Vector2<T> p1, Vector2<T> p2) {
        return fromCoords(p1.m_x, p1.m_y, p2.m_x, p2.m_y);
    }

    constexpr static Rectangle fromSize(T x1, T y1, T width, T height) {
        return {x1, y1, x1 + width, y1 + height};
    }

    template<typename U>
    constexpr static Rectangle fromOther(Rectangle<U>& other) {
        return {static_cast<T>(other.getX1()), static_cast<T>(other.getY1()), static_cast<T>(other.getX2()), static_cast<T>(other.getY2())};
    }

    [[nodiscard]] constexpr T getX1() const {
        return m_x1;
    }

    [[nodiscard]] constexpr T getX2() const {
        return m_x2;
    }

    [[nodiscard]] constexpr T getY1() const {
        return m_y1;
    }

    [[nodiscard]] constexpr T getY2() const {
        return m_y2;
    }

    [[nodiscard]] constexpr T getWidth() const {
        return m_x2 - m_x1;
    }

    [[nodiscard]] constexpr T getHeight() const {
        return m_y2 - m_y1;
    }

    [[nodiscard]] constexpr Vector2<T> getP1() {
        return {m_x1, m_y1};
    }

    [[nodiscard]] constexpr Vector2<T> getP2() {
        return {m_x2, m_y2};
    }
};

using RectangleF = Rectangle<float>;
using RectangleI = Rectangle<int>;