#pragma once

#include <SFML/Graphics/Color.hpp>

class BaseStick;

class BasicStickDetails {
public:
    BaseStick* m_parent = nullptr;
    int m_id1;
    int m_id2;
    float m_length;
    sf::Color m_color = sf::Color::White;

    BasicStickDetails(int id1, int id2, float length) : m_id1(id1), m_id2(id2), m_length(length){}
};

class BasicStickDetails;

class BaseStick {
private:
    BasicStickDetails* m_basicStickDetails = nullptr;
    int m_id = -1;
public:
    [[nodiscard]] BasicStickDetails& getBasicStickDetails() const {
        return *m_basicStickDetails;
    }

    virtual void v_onTick() = 0;
    virtual void v_onInit() = 0;

    virtual ~BaseStick() = default;

    friend class StickStorage;
};