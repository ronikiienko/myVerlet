#pragma once

#include "../Engine/EventBus/BaseEvent.h"

struct PlayerBulletCountUpdate : public BaseEvent {
    int m_newCount;
    explicit PlayerBulletCountUpdate(int newCount) : m_newCount(newCount) {}
};