#pragma once

class IDamageable {
public:
    virtual void receiveDamage(float damage) = 0;
};