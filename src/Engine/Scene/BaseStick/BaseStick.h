#pragma once

#include "BasicStickDetails.h"

class BasicStickDetails;

class BaseStick {
    BasicStickDetails* m_basicStickDetails;
public:
    [[nodiscard]] BasicStickDetails& getBasicStickDetails() const {
        return *m_basicStickDetails;
    }

    int m_id;
    virtual void v_onTick() = 0;
    virtual void v_onInit() = 0;

    virtual ~BaseStick() = default;

    friend class StickStorage;
};