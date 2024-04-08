#pragma once

#include <vector>
#include "BaseStick/BaseStick.h"

class StickStorage {
private:
    int m_maxSticksNum;
    std::vector<BasicStickDetails> m_basicSticksDetails;
    std::vector<std::shared_ptr<BaseStick>> m_sticks;
    std::set<int, std::greater<>> m_sticksToRemove;
    std::vector<std::set<int>> m_sticksOfObjects; // each index is object id, each set contains stick ids of this object
    ObjectStorage &m_objectStorage;
    int m_elementTransferCallbackId = -1;

    void markStickForRemoval(int index) {
        m_sticksToRemove.insert(index);
    }
public:
    template<typename T>
    void forEachStick(const T &callback, int start = 0, int end = -1) {
        // as i've discovered.. std::function HAS overhead. It was taking around 2 fps. So, i use T here. for performance-critical code parts better to not use std::function
        if (end == -1) {
            end = getSticksCount();
        }

        for (int i = start; i < end; i++) {
            callback(*m_sticks[i], i);
        }
    }

    template<typename T>
    void forEachBasicStickDetails(const T &callback, int start = 0, int end = -1) {
        if (end == -1) {
            end = static_cast<int>(m_basicSticksDetails.size());
        }

        for (int i = start; i < end; i++) {
            callback(m_basicSticksDetails[i], i);
        }
    }


    template<typename T>
    std::weak_ptr<BaseStick> addStick(T &&stick, int id1, int id2, float length) {
        if (m_sticks.size() + 1 > m_maxSticksNum) {
            throw std::runtime_error("StickStorage: max sticks num reached");
        }

        m_basicSticksDetails.emplace_back(id1, id2, length);
        stick.m_basicStickDetails = &m_basicSticksDetails.back();

        m_sticks.push_back(std::make_shared<T>(std::forward<T>(stick)));

        int index = static_cast<int>(m_sticks.size() - 1);
        m_basicSticksDetails[index].m_parent = m_sticks[index].get();
        m_sticks[index]->m_id = index;
        m_sticks[index]->v_onInit();

        m_sticksOfObjects[id1].insert(index);
        m_sticksOfObjects[id2].insert(index);

        return {m_sticks[index]};
    }

    [[nodiscard]] int getSticksCount() const {
        return static_cast<int>(m_basicSticksDetails.size());
    }

    void removeStick(int index) {
        markStickForRemoval(index);
    }

    void removeMarkedSticks() {
        if (m_sticksToRemove.empty()) {
            return;
        }

        for (const auto &i : m_sticksToRemove) {
            int lastIndex = getSticksCount() - 1;
            if (i > lastIndex || i < 0) {
                throw std::runtime_error("StickStorage: trying to remove stick with invalid index");
            }

            m_sticksOfObjects[m_basicSticksDetails[i].m_id1].erase(i);
            m_sticksOfObjects[m_basicSticksDetails[i].m_id2].erase(i);

            if (i == lastIndex) {
                m_sticks.pop_back();
                m_basicSticksDetails.pop_back();
            }

            if (i < lastIndex) {
                std::swap(m_sticks[i], m_sticks[lastIndex]);
                std::swap(m_basicSticksDetails[i], m_basicSticksDetails[lastIndex]);
                m_sticks.pop_back();
                m_basicSticksDetails.pop_back();
                m_sticks[i]->m_basicStickDetails = &m_basicSticksDetails[i];
                m_sticks[i]->m_id = i;
            }
        }
        m_sticksToRemove.clear();
    }

    explicit StickStorage(int maxSticksNum, ObjectStorage &objectStorage) :
            m_maxSticksNum(maxSticksNum),
            m_objectStorage(objectStorage) {
        m_sticks.reserve(m_maxSticksNum);
        m_basicSticksDetails.reserve(m_maxSticksNum);
        m_sticksOfObjects.resize(objectStorage.getMaxObjectsCount());
        m_elementTransferCallbackId = m_objectStorage.addElementTransferCallback([this](int id, int newId) {
            if (newId == -1) {
                for (int stickId : m_sticksOfObjects[id]) {
                    removeStick(stickId);
                }
            } else {
                for (int stickId: m_sticksOfObjects[id]) {
                    if (m_basicSticksDetails[stickId].m_id1 == id) {
                        m_basicSticksDetails[stickId].m_id1 = newId;
                    } else {
                        m_basicSticksDetails[stickId].m_id2 = newId;
                    }
                }
            }
        });
    }

    ~StickStorage() {
        m_objectStorage.removeElementTransferCallback(m_elementTransferCallbackId);
    }
};