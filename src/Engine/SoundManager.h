#pragma once
#include <SFML/Audio.hpp>

class SoundManager {
    sf::Music m_music;

public:
    void play(const std::string& path) {
        if (!m_music.openFromFile(path)) {
            throw std::runtime_error("Sound file could not be loaded" + path);
        }
        m_music.play();
    }
};