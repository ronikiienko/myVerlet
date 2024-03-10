#pragma once

#include <iostream>

class BoidsBlackboard {
public:
    float m_alignRadius = 30;
    float m_cohesionRadius = 30;
    float m_separationRadius = 10;

    float m_separationWeight = 1;
    float m_alignWeight = 1;
    float m_cohesionWeight = 1;

    float m_maxAcceleration = 1;

    float m_minVelocity = 0.1;
    float m_maxVelocity = 0.15;
};