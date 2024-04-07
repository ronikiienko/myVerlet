#pragma once

#include <SFML/Graphics/Color.hpp>

class BaseStick {
public:
    int m_id1;
    int m_id2;
    float m_length;
    sf::Color m_color = sf::Color::White;

    BaseStick(int id1, int id2, float length) : m_id1(id1), m_id2(id2), m_length(length){}
};