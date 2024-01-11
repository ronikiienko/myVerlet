#pragma once

#include <cmath>

class Angle {
private:
    float m_degrees;

    constexpr explicit Angle(float degrees) : m_degrees(degrees) {}

public:
    constexpr static Angle fromDegrees(float degrees) {
        return Angle{degrees};
    }

    constexpr static Angle fromRadians(float radians) {
        return Angle{radians * 180.0f / static_cast<float>(M_PI)};
    }

    [[nodiscard]] constexpr float getDegrees() const {
        return m_degrees;
    }

    [[nodiscard]] constexpr float getRadians() const {
        return m_degrees * static_cast<float>(M_PI) / 180.0f;
    }

    constexpr void operator+=(const Angle &other) {
        m_degrees += other.m_degrees;
    }

    constexpr Angle operator+(const Angle &other)const {
        return Angle::fromDegrees(m_degrees + other.m_degrees);
    }

    constexpr void operator-=(const Angle &other) {
        m_degrees -= other.m_degrees;
    }

    constexpr Angle operator-(const Angle &other)const {
        return Angle::fromDegrees(m_degrees - other.m_degrees);
    }
};

