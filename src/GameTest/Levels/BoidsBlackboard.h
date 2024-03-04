#pragma once

#include <iostream>

class BoidsBlackboard {
public:
    float m_alignRadius = 30;
    float m_cohesionRadius = 30;
    float m_separationRadius = 10;

    float m_separationWeight = 0.2;
    float m_alignWeight = 0.2;
    float m_cohesionWeight = 0.2;

    float m_maxAcceleration = 3;
};