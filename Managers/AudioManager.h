#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

class AudioManager {
public:
    static AudioManager& GetInstance() {
        static AudioManager instance;
        return instance;
    }

    void PlaySound(const std::string& soundPath) {
        if (soundBuffers.find(soundPath) == soundBuffers.end()) {
            sf::SoundBuffer buffer;
            if (!buffer.loadFromFile(soundPath)) {
                return; 
            }
            soundBuffers[soundPath] = std::move(buffer);
        }

        sf::Sound sound;
        sound.setBuffer(soundBuffers[soundPath]);
        sounds.push_back(std::move(sound));
        sounds.back().play();
    }

    void SetMusic(const std::string& musicPath, bool loop = true, float volume = 50.0f) {
        if (!music.openFromFile(musicPath)) {
            return; 
        }
        music.setLoop(loop);
        music.setVolume(volume);
        music.play();
    }

    void StopMusic() {
        music.stop();
    }

    void SetMusicVolume(float volume) {
        music.setVolume(volume);
    }

private:
    AudioManager() = default;
    ~AudioManager() = default;

    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::vector<sf::Sound> sounds;
    sf::Music music;
};
