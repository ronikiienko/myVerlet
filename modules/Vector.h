#pragma once

#include <cmath>
#include <iostream>

class Vector2 {
public:
    float x, y;

    explicit Vector2(float x = 0, float y = 0) : x(x), y(y) {}

    Vector2 operator-(const Vector2 &other) const {
        return Vector2{x - other.x, y - other.y};
    }

    Vector2 operator+(const Vector2 &other) const {
        return Vector2{x + other.x, y + other.y};
    }

    Vector2 operator*(float scalar) const {
        return Vector2{x * scalar, y * scalar};
    }

    Vector2 operator/(float scalar) const {
        return Vector2{x / scalar, y / scalar};
    }

    [[nodiscard]] float dot(const Vector2 &other) const {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] float magnitude2() const {
        return x * x + y * y;
    }

    [[nodiscard]] Vector2 normalize() const {
        float mag = magnitude();
        return Vector2{x / mag, y / mag};
    }

    void log() const {
        std::cout << '\n' << "X: " << x << " " << "Y: " << y << '\n';
    }
};