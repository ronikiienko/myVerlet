#pragma once

#include "../Engine/EventBus/BaseEvent.h"

namespace GameEvents {
    struct PlayerBulletCountUpdate : public BaseEvent {
        int m_newCount;
        explicit PlayerBulletCountUpdate(int newCount) : m_newCount(newCount) {}
    };
    struct EnemyCountUpdate : public BaseEvent {
        int m_newCount;
        explicit EnemyCountUpdate(int newCount) : m_newCount(newCount) {}
    };
}


