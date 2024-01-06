#pragma once

#include <cmath>

class Angle {
private:
    float degrees;

    constexpr explicit Angle(float degrees) : degrees(degrees) {}

public:
    constexpr static Angle fromDegrees(float degrees) {
        return Angle{degrees};
    }

    constexpr static Angle fromRadians(float radians) {
        return Angle{radians * 180.0f / static_cast<float>(M_PI)};
    }

    [[nodiscard]] constexpr float getDegrees() const {
        return degrees;
    }

    [[nodiscard]] constexpr float getRadians() const {
        return degrees * static_cast<float>(M_PI) / 180.0f;
    }

    constexpr void operator+=(const Angle &other) {
        degrees += other.degrees;
    }

    constexpr Angle operator+(const Angle &other)const {
        return Angle::fromDegrees(degrees + other.degrees);
    }

    constexpr void operator-=(const Angle &other) {
        degrees -= other.degrees;
    }

    constexpr Angle operator-(const Angle &other)const {
        return Angle::fromDegrees(degrees - other.degrees);
    }
};

