#pragma once

#include <cmath>

class Angle {
private:
    float degrees;

    constexpr explicit Angle(float degrees) : degrees(degrees) {}

public:
    static Angle fromDegrees(float degrees) {
        return Angle{degrees};
    }

    static Angle fromRadians(float radians) {
        return Angle{radians * 180.0f / static_cast<float>(M_PI)};
    }

    [[nodiscard]] float getDegrees() const {
        return degrees;
    }

    [[nodiscard]] float getRadians() const {
        return degrees * static_cast<float>(M_PI) / 180.0f;
    }
};
