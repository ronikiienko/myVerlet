#pragma once

#include "../Engine/Scene.h"
#include "../Engine/EngineConsts.h"
#include "../Objects/Bullet.h"


class Shooter {
private:
    Vector2F position;
    Angle direction = Angle::fromDegrees(0);
    float speed;
    int width;
    int rowsNum;
    float interval;
    Scene &scene;
    ExplosionHandler& explosionHandler;
public:


    Shooter(Vector2F position, float speed, int width, float interval, Scene &scene,
            int rowsNum, ExplosionHandler& explosionHandler) : position(position), speed(speed), width(width), interval(interval),
                                                               scene(scene), rowsNum(rowsNum), explosionHandler(explosionHandler) {}

    void rotate(Angle angle) {
        direction += angle;
    }

    void setDirection(Angle angle) {
        direction = angle;
    }

    void move(Vector2F vector) {
        position += vector;
    }

    void setPosition(Vector2F vector) {
        position = vector;
    }

    void setWidth(int value) {
        width = value;
    }


    void shoot(Vector2F target) {
        pointTo(target);
        Vector2F rowOffset = Vector2F::cart(0, 0);
        for (int row = 0; row < rowsNum; row++) {
            const Angle rowDirection = direction + Angle::fromDegrees(90);
            const Vector2F move = Vector2F::polar(interval, rowDirection);
            const Vector2F initialVelocity = Vector2F::polar(speed, direction);
            const int leftOffset = width / 2;
            Vector2F currentPosition = position - (move * static_cast<float>(leftOffset)) + rowOffset;
            for (int i = 0; i < width; i++) {
                auto ptr = scene.addObject(Bullet(explosionHandler), currentPosition + Vector2F::cart(5, 5));
                ptr.lock()->basicDetails->setVelocity(initialVelocity);
                currentPosition += move;
            }
            rowOffset += Vector2F::polar(interval, direction);
        }
    }

    void pointTo(Vector2F point) {
        direction = Angle::fromRadians(std::atan2(point.y - position.y, point.x - position.x));
    }
};