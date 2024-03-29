#pragma once

#include <memory>
#include "SoundManagerImpl.h"

class SoundManager {
private:
    std::shared_ptr<SoundManagerImpl> m_impl;
public:
    SoundManager() : m_impl(std::make_shared<SoundManagerImpl>()) {}

    [[nodiscard]] SMHandle play(const std::string& path) {
        return m_impl->play(path);
    }
};