#pragma once
#include <SFML/Audio.hpp>
#include "SMHandle.h"
#include <unordered_map>

class SoundManagerImpl : public std::enable_shared_from_this<SoundManagerImpl> {
    std::unordered_map<int, std::pair<sf::SoundBuffer, sf::Sound>> m_sounds;
    int m_keyCounter = 0;
public:
    SoundManagerImpl() = default;

    [[nodiscard]] SMHandle play(const std::string& path) {
        m_sounds[m_keyCounter].first.loadFromFile(path);
        m_sounds[m_keyCounter].second.setBuffer(m_sounds[m_keyCounter].first);
        return {weak_from_this(), m_keyCounter++};
    }

    void remove(int key) {
        m_sounds.erase(key);
    }

    SoundManagerImpl(const SoundManagerImpl &) = delete;
    SoundManagerImpl& operator=(const SoundManagerImpl &) = delete;
    SoundManagerImpl(SoundManagerImpl &&) = delete;
    SoundManagerImpl& operator=(SoundManagerImpl &&) = delete;
};