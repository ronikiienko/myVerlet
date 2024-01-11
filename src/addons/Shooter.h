#pragma once

#include "../Engine/Scene.h"
#include "../Engine/EngineConsts.h"
#include "../Objects/Bullet.h"


class Shooter {
private:
    Vector2F m_position;
    Angle m_direction = Angle::fromDegrees(0);
    float m_speed;
    int m_width;
    int m_rowsNum;
    float m_interval;
    Scene &m_scene;
    ExplosionHandler& m_explosionHandler;
public:


    Shooter(Vector2F position, float speed, int width, float interval, Scene &scene,
            int rowsNum, ExplosionHandler& explosionHandler) : m_position(position), m_speed(speed), m_width(width), m_interval(interval),
                                                               m_scene(scene), m_rowsNum(rowsNum), m_explosionHandler(explosionHandler) {}

    void rotate(Angle angle) {
        m_direction += angle;
    }

    void setDirection(Angle angle) {
        m_direction = angle;
    }

    void move(Vector2F vector) {
        m_position += vector;
    }

    void setPosition(Vector2F vector) {
        m_position = vector;
    }

    void setWidth(int value) {
        m_width = value;
    }


    void shoot(Vector2F target) {
        pointTo(target);
        Vector2F rowOffset = Vector2F::cart(0, 0);
        for (int row = 0; row < m_rowsNum; row++) {
            const Angle rowDirection = m_direction + Angle::fromDegrees(90);
            const Vector2F move = Vector2F::polar(m_interval, rowDirection);
            const Vector2F initialVelocity = Vector2F::polar(m_speed, m_direction);
            const int leftOffset = m_width / 2;
            Vector2F currentPosition = m_position - (move * static_cast<float>(leftOffset)) + rowOffset;
            for (int i = 0; i < m_width; i++) {
                auto ptr = m_scene.addObject(Bullet(m_explosionHandler), currentPosition + Vector2F::cart(5, 5));
                ptr.lock()->m_basicDetails->setVelocity(initialVelocity);
                currentPosition += move;
            }
            rowOffset += Vector2F::polar(m_interval, m_direction);
        }
    }

    void pointTo(Vector2F point) {
        m_direction = Angle::fromRadians(std::atan2(point.m_y - m_position.m_y, point.m_x - m_position.m_x));
    }
};