#pragma once

#include <cmath>
#include <iostream>
#include "Angle.h"

template<typename T>
class Vector2 {
private:
    constexpr explicit Vector2(T x = 0, T y = 0) : m_x(x), m_y(y) {}

public:
    T m_x, m_y;

    constexpr static Vector2 cart(T x = 0, T y = 0) {
        return Vector2{x, y};
    }

    constexpr static Vector2 polar(T magnitude, const Angle &angle) {
        T angleRadians = angle.getRadians();

        T x = magnitude * std::cos(angleRadians);
        T y = magnitude * std::sin(angleRadians);

        return Vector2{x, y};
    }

    constexpr Vector2 operator-(const Vector2 &other) const {
        return Vector2{m_x - other.m_x, m_y - other.m_y};
    }

    constexpr Vector2 operator+(const Vector2 &other) const {
        return Vector2{m_x + other.m_x, m_y + other.m_y};
    }

    constexpr Vector2 operator*(T scalar) const {
        return Vector2{m_x * scalar, m_y * scalar};
    }

    constexpr Vector2 operator/(T scalar) const {
        // TODO remove safety checks
        if (scalar == 0) {
            throw std::runtime_error("Trying to divide by 0 (/)");
        }
        return Vector2{m_x / scalar, m_y / scalar};
    }

    [[nodiscard]] constexpr T dot(const Vector2 &other) const {
        return m_x * other.m_x + m_y * other.m_y;
    }

    [[nodiscard]] constexpr T magnitude() const {
        return std::sqrt(m_x * m_x + m_y * m_y);
    }

    [[nodiscard]] constexpr T magnitude2() const {
        return m_x * m_x + m_y * m_y;
    }

    [[nodiscard]] constexpr Vector2 normalize() const {
        T mag = magnitude();
        return Vector2{m_x / mag, m_y / mag};
    }

    void log() const {
        std::cout << '\n' << "X: " << m_x << " " << "Y: " << m_y << '\n';
    }

    // Add the += operator
    constexpr Vector2 &operator+=(const Vector2 &other) {
        m_x += other.m_x;
        m_y += other.m_y;
        return *this;
    }

    // Add the -= operator
    constexpr Vector2 &operator-=(const Vector2 &other) {
        m_x -= other.m_x;
        m_y -= other.m_y;
        return *this;
    }

    // Add the -= operator
    constexpr Vector2 &operator/=(const T scalar) {
        // TODO remove safety checks
        if (scalar == 0) {
            throw std::runtime_error("Trying to divide by 0 (/=)");
        }
        m_x /= scalar;
        m_y /= scalar;
        return *this;
    }

    // Add the -= operator
    constexpr Vector2 &operator*=(const T scalar) {
        m_x *= scalar;
        m_y *= scalar;
        return *this;
    }

    constexpr void limitMagnitude(T limit) {
        const T mag2 = magnitude2();
        if (mag2 > limit * limit) {
            const T scale = limit / std::sqrt(mag2);
            m_x *= scale;
            m_y *= scale;
        }
    }

    void addX(T xAdd) {
        m_x += xAdd;
    }
    void addY(T yAdd) {
        m_y += yAdd;
    }

    template<typename U>
    constexpr static Vector2 fromOther(Vector2<U> other) {
        return Vector2{static_cast<T>(other.m_x), static_cast<T>(other.m_y)};
    }
};

using Vector2F = Vector2<float>;
using Vector2I = Vector2<int>;