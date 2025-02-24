#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <map>
#include <string>



enum class FruitType {
    Avocado,
    Strawberry,
    Lemon
};


class TextureManager {
public:
    static TextureManager& GetInstance() {
        static TextureManager instance;
        return instance;
    }

   sf::Texture& GetTexture(FruitType type) {
        if (textures.find(type) == textures.end()) {
            LoadTexture(type);
        }
        return textures[type];
    }

private:
    TextureManager() {} 
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    void LoadTexture(FruitType type) {
        std::string texturePath;
        switch (type) {
        case FruitType::Avocado:
            texturePath = "Textures/avokado.png";
            break;
        case FruitType::Strawberry:
            texturePath = "Textures/limon.png";
            break;
        case FruitType::Lemon:
            texturePath = "Textures/klubnika-tenm.png";
            break;
        }

        sf::Texture texture;
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
            
        }
        textures[type] = texture;
    }

    std::map<FruitType, sf::Texture> textures;
};
