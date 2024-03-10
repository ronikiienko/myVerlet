#pragma once

#include "../Camera.h"
#include "../utils/Vector.h"

class CameraFollowing {
    Camera &m_camera;
    float m_transitionCoefficient = 1;
public:
    explicit CameraFollowing(Camera &camera) : m_camera(camera) {}

    void setTransitionCoefficient(float amount) {
        m_transitionCoefficient = amount;
    }

    void follow(Vector2F newObjectPosition) {
        Vector2F oldPosition = m_camera.getPosition();
        Vector2F difference = newObjectPosition - oldPosition;
        Vector2F newPosition = oldPosition + difference * m_transitionCoefficient;
        m_camera.setPosition(newPosition);
    }
};