#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

#include "../Core/Vector2D.h"


class Object {
public:
    Object(const std::string& TexturePath = "") {
        if (!TexturePath.empty() && !LoadTexture(TexturePath)) {
            std::cerr << "Ошибка загрузки текстуры: " << TexturePath << std::endl;
        }
    }

    Object(sf::Texture& texture) {
        Texture = texture;
        Sprite.setTexture(Texture);
    }

    virtual ~Object() = default;

    void SetPosition(const Vector2D& position)
    {
        Position = position;
        Sprite.setPosition(Position.ToSFML()); 
    }


    void Move(const Vector2D& Delta) {
        Position.X += Delta.X;
        Position.Y += Delta.Y;
        Sprite.setPosition(Position.ToSFML());
    }
    
    void SetSize(float width, float height) {
        sf::FloatRect bounds = Sprite.getLocalBounds();
        Vector2D scale(width / bounds.width, height / bounds.height);
        SetScale(scale); 
    }

    Vector2D GetSize() const {
        sf::FloatRect bounds = Sprite.getLocalBounds();
        Vector2D scale = GetScale();
        return Vector2D(bounds.width * scale.X, bounds.height * scale.Y);
    }


    void SetScale(const Vector2D& scale) {
        Scale = scale;
        Sprite.setScale(Scale.ToSFML());
    }


    Vector2D GetScale() const { return Scale; }

    void SetRotation(float angle) {
        Rotation = angle;
        Sprite.setRotation(Rotation);
    }

    virtual void Draw(sf::RenderWindow& window) {
        if (IsVisible) {
            window.draw(Sprite);
        }
    }

    bool LoadTexture(const std::string& FilePath) {
        if (!Texture.loadFromFile(FilePath)) {
            return false;
        }
        Sprite.setTexture(Texture);
        return true;
    }


    float GetRotation() const { return Rotation; }
    bool GetIsVisible() const { return IsVisible; }
    Vector2D GetPosition() const { return Position; }

    void SetIsVisible(bool visible) { IsVisible = visible; }

protected:
    sf::Texture Texture;
    sf::Sprite Sprite;
    Vector2D Position = {100.f, 0.f};
    Vector2D Scale = {1.f, 1.f};
    float Rotation = 0.f;
    bool IsVisible = true;
};