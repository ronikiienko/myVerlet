#pragma once

#include "../modules/Angle.h"
#include "../modules/Vector.h"
#include "../World/World.h"
#include "../consts.h"

class Shooter {
private:
    Vector2 position;
    Angle direction;
    float speed;
    int width;
    int rowsNum;
    float interval;
    World& world;
public:


    Shooter(Vector2 position, Angle direction, float speed, int width, float interval, World& world, int rowsNum) : position(position), direction(direction), speed(speed), width(width), interval(interval), world(world), rowsNum(rowsNum) {}

    void rotate(Angle angle) {
        direction += angle;
    }

    void setDirection(Angle angle) {
        direction = angle;
    }

    void move(Vector2 vector) {
        position += vector;
    }

    void setPosition(Vector2 vector) {
        position = vector;
    }

    void setWidth(int value) {
        width = value;
    }


    void shoot() {
        Vector2 rowOffset = Vector2::fromCartesian(0,0);
        for (int row = 0; row < rowsNum; row++) {
            const Angle rowDirection = direction + Angle::fromDegrees(90);
            const Vector2 move = Vector2::fromPolar(interval, rowDirection);
            const Vector2 initialVelocity = Vector2::fromPolar(speed, direction);
            const int leftOffset = width / 2;
            Vector2 currentPosition = position - (move * static_cast<float>(leftOffset)) + rowOffset;
            for (int i = 0; i < width; i++) {
                VerletObject& object = world.addObject(currentPosition, maxRadius);
                object.setVelocity(initialVelocity);
                currentPosition += move;
            }
            rowOffset += Vector2::fromPolar(interval, direction);
        }
    }
};