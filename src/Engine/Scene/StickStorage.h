#pragma once

#include <vector>
#include "BaseStick/BaseStick.h"

class StickStorage {
private:
    std::vector<BaseStick> m_baseSticks;
public:
    template<typename T>
    void forEachStick(const T &callback, int start = 0, int end = -1) {
        // as i've discovered.. std::function HAS overhead. It was taking around 2 fps. So, i use T here. for performance-critical code parts better to not use std::function
        if (end == -1) {
            end = static_cast<int>(m_baseSticks.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_baseSticks[i], i);
        }
    }

    void addStick(BaseStick stick) {
        m_baseSticks.push_back(stick);
    }
};