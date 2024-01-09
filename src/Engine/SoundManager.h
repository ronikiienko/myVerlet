#pragma once
#include <SFML/Audio.hpp>

class SoundManager {
    sf::Music music;

public:
    void play(const std::string& path) {
        if (!music.openFromFile("sound.wav")) {
            throw std::runtime_error("Sound file could not be loaded" + path);
        }
        music.play();
    }
};