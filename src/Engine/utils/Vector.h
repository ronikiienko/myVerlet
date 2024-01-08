#pragma once

#include <cmath>
#include <iostream>
#include "Angle.h"

class Vector2 {
private:
    constexpr explicit Vector2(float x = 0, float y = 0) : x(x), y(y) {}

public:
    float x, y;

    constexpr static Vector2 cart(float x = 0, float y = 0) {
        return Vector2{x, y};
    }

    constexpr static Vector2 polar(float magnitude, const Angle &angle) {
        float angleRadians = angle.getRadians();

        float x = magnitude * std::cos(angleRadians);
        float y = magnitude * std::sin(angleRadians);

        return Vector2{x, y};
    }

    constexpr Vector2 operator-(const Vector2 &other) const {
        return Vector2{x - other.x, y - other.y};
    }

    constexpr Vector2 operator+(const Vector2 &other) const {
        return Vector2{x + other.x, y + other.y};
    }

    constexpr Vector2 operator*(float scalar) const {
        return Vector2{x * scalar, y * scalar};
    }

    constexpr Vector2 operator/(float scalar) const {
        // TODO remove safety checks
        if (scalar == 0) {
            throw std::runtime_error("Trying to divide by 0 (/)");
        }
        return Vector2{x / scalar, y / scalar};
    }

    [[nodiscard]] constexpr float dot(const Vector2 &other) const {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] constexpr float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr float magnitude2() const {
        return x * x + y * y;
    }

    [[nodiscard]] constexpr Vector2 normalize() const {
        float mag = magnitude();
        return Vector2{x / mag, y / mag};
    }

    void log() const {
        std::cout << '\n' << "X: " << x << " " << "Y: " << y << '\n';
    }

    // Add the += operator
    constexpr Vector2 &operator+=(const Vector2 &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Add the -= operator
    constexpr Vector2 &operator-=(const Vector2 &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    // Add the -= operator
    constexpr Vector2 &operator/=(const float scalar) {
        // TODO remove safety checks
        if (scalar == 0) {
            throw std::runtime_error("Trying to divide by 0 (/=)");
        }
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Add the -= operator
    constexpr Vector2 &operator*=(const float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr void limitMagnitude(float limit) {
        const float mag2 = magnitude2();
        if (mag2 > limit * limit) {
            const float scale = limit / std::sqrt(mag2);
            x *= scale;
            y *= scale;
        }
    }

    void addX(float xAdd) {
        x += xAdd;
    }
    void addY(float yAdd) {
        y += yAdd;
    }
};