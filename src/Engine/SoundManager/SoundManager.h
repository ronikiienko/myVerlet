#pragma once
#include <SFML/Audio.hpp>

class SoundManager {
    sf::Music m_music;

public:
    SoundManager() = default;

    void play(const std::string& path) {
        if (!m_music.openFromFile(path)) {
            throw std::runtime_error("Sound file could not be loaded" + path);
        }
        m_music.play();
    }

    SoundManager(const SoundManager &) = delete;
    SoundManager& operator=(const SoundManager &) = delete;
    SoundManager(SoundManager &&) = delete;
    SoundManager& operator=(SoundManager &&) = delete;
};