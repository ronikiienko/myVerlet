#pragma once

class IDamageable {
public:
    virtual void receiveDamage(int damage) = 0;
};