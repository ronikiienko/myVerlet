#pragma once

#include <cmath>
#include <iostream>
#include <algorithm>
#include "Angle.h"

template<typename T>
class Vector2 {
private:


public:
    T m_x = 0, m_y = 0;

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

    constexpr Vector2 operator-() const {
        return Vector2{-m_x, -m_y};
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

    [[nodiscard]] constexpr T perpClockwise() {
        return Vector2{m_y, -m_x};
    }

    [[nodiscard]] constexpr T perpCounterClockwise() {
        return Vector2{-m_y, m_x};
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

    [[nodiscard]] constexpr Vector2 rotate(const Angle &angle) const {
        T angleRadians = angle.getRadians();
        T cos = std::cos(angleRadians);
        T sin = std::sin(angleRadians);

        return Vector2{m_x * cos - m_y * sin, m_x * sin + m_y * cos};
    }

    [[nodiscard]] constexpr Vector2 min(const Vector2 &other) const {
        return Vector2{std::min(m_x, other.m_x), std::min(m_y, other.m_y)};
    }

    [[nodiscard]] constexpr Vector2 max(const Vector2 &other) const {
        return Vector2{std::max(m_x, other.m_x), std::max(m_y, other.m_y)};
    }

    [[nodiscard]] constexpr Vector2 clamp(const Vector2 &min, const Vector2 &max) const {
        return Vector2{std::clamp(m_x, min.m_x, max.m_x), std::clamp(m_y, min.m_y, max.m_y)};
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

    constexpr void limitMagnitudeModify(T limit) {
        const T mag2 = magnitude2();
        if (mag2 > limit * limit) {
            const T scale = limit / std::sqrt(mag2);
            m_x *= scale;
            m_y *= scale;
        }
    }

    constexpr Vector2<T> limitMagnitude(T limit) const {
        const T mag2 = magnitude2();
        if (mag2 > limit * limit) {
            const T scale = limit / std::sqrt(mag2);
            return Vector2::cart(m_x * scale, m_y * scale);
        } else {
            return Vector2::cart(m_x, m_y);
        }
    }

    constexpr void limitByXSaveRatio(T limit) {
        static_assert(std::is_floating_point<T>::value, "limitByXSaveRatio only works with floating point types");
        float oldRatio = static_cast<T>(m_y) / static_cast<T>(m_x);
        m_x = std::min(m_x, limit);
        m_x = std::max(m_x, -limit);
        m_y = m_x * oldRatio;
    }

    constexpr void limitByYSaveRatio(T limit) {
        static_assert(std::is_floating_point<T>::value, "limitByYSaveRatio only works with floating point types");
        float oldRatio = static_cast<T>(m_x) / static_cast<T>(m_y);
        m_y = std::min(m_y, limit);
        m_y = std::max(m_y, -limit);
        m_x = m_y * oldRatio;
    }

    constexpr void limitByXYSaveRatio(T maxX, T maxY) {
        static_assert(std::is_floating_point<T>::value, "limitByXYSaveRatio only works with floating point types");
        limitByXSaveRatio(maxX);
        limitByYSaveRatio(maxY);
    }

    void addX(T xAdd) {
        m_x += xAdd;
    }

    void addY(T yAdd) {
        m_y += yAdd;
    }

    [[nodiscard]] Angle getAngle() const {
        return Angle::fromRadians(std::atan2(m_y, m_x));
    }

    template<typename U>
    constexpr static Vector2 fromOther(Vector2<U> other) {
        return Vector2{static_cast<T>(other.m_x), static_cast<T>(other.m_y)};
    }

    constexpr explicit Vector2(T x = 0, T y = 0) : m_x(x), m_y(y) {}
};

using Vector2F = Vector2<float>;
using Vector2I = Vector2<int>;