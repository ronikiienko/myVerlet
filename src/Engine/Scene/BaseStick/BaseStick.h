#pragma once

#include <SFML/Graphics/Color.hpp>

class BaseStick;

class BasicStickDetails {
public:
    BaseStick* m_parent = nullptr;
    int m_id1;
    int m_id2;
    float m_length;
    float m_maxStretch = 100;
    float m_stiffness = 1;
    float m_currentStretch = 1;
    sf::Color m_color = sf::Color::White;

    BasicStickDetails(int id1, int id2, float length) : m_id1(id1), m_id2(id2), m_length(length){}
};

class BaseStick {
private:
    BasicStickDetails* m_basicStickDetails = nullptr;
    int m_id = -1;
public:
    [[nodiscard]] BasicStickDetails& getBasicStickDetails() const {
#ifdef IT_IS_DEBUG
        if (m_id == -1) {
            throw std::runtime_error("Trying to get stick basic details, but it's not initialized.");
        }
#endif
        return *m_basicStickDetails;
    }

    [[nodiscard]] int getId() const {
#ifdef IT_IS_DEBUG
        if (m_id == -1) {
            throw std::runtime_error("Trying to get stick id, but it's not set.");
            }
#endif
        return m_id;
    }

    virtual void v_onTick() = 0;
    virtual void v_onInit() = 0;

    virtual ~BaseStick() = default;

    friend class StickStorage;
};