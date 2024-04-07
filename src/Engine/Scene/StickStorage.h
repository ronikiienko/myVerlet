#pragma once

#include <vector>
#include "BaseStick/BaseStick.h"

class StickStorage {
private:
    int m_maxSticksNum;
    std::vector<BasicStickDetails> m_basicSticksDetails;
    std::vector<std::shared_ptr<BaseStick>> m_sticks;
    std::set<int, std::greater<>> m_sticksToRemove;
public:
    template<typename T>
    void forEachStick(const T &callback, int start = 0, int end = -1) {
        // as i've discovered.. std::function HAS overhead. It was taking around 2 fps. So, i use T here. for performance-critical code parts better to not use std::function
        if (end == -1) {
            end = static_cast<int>(m_basicSticksDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_basicSticksDetails[i], i);
        }
    }

    template<typename T>
    std::weak_ptr<BaseStick> addStick(T&& object, int id1, int id2, float length) {
        if (m_sticks.size() + 1 > m_maxSticksNum) {
            throw std::runtime_error("StickStorage: max sticks num reached");
        }

        m_basicSticksDetails.emplace_back(id1, id2, length);
        object.m_basicStickDetails = &m_basicSticksDetails.back();

        m_sticks.push_back(std::make_shared<T>(std::forward<T>(object)));

        int index = static_cast<int>(m_sticks.size() - 1);
        m_basicSticksDetails[index].m_parent = m_sticks[index].get();
        m_sticks[index]->m_id = index;
        m_sticks[index]->v_onInit();

        return {m_sticks[index]};
    }

    [[nodiscard]] int getSticksCount() const {
        return static_cast<int>(m_basicSticksDetails.size());
    }

    explicit StickStorage(int maxSticksNum) : m_maxSticksNum(maxSticksNum) {
        m_sticks.reserve(m_maxSticksNum);
        m_basicSticksDetails.reserve(m_maxSticksNum);
    }
};