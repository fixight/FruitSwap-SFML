#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "../Objects/Object.h"

class UIElementBase : public Object {
public:
    UIElementBase(const std::string& TexturePath = "") : Object(TexturePath) {
        if (!TexturePath.empty()) {
            Sprite.setTexture(Texture);
        }
        Sprite.setColor(sf::Color::White);
    }

    UIElementBase(sf::Texture& texture) : Object(texture) {
        Texture = texture;
        Sprite.setTexture(Texture);
    }

    bool IsMouseOver(const sf::RenderWindow& window) const {
        sf::FloatRect bounds = Sprite.getGlobalBounds();
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    }

    void Draw(sf::RenderWindow& window) {
        if(IsVisible)
            window.draw(Sprite);
    }

    void HandleClick(const sf::RenderWindow& window) {
        if (IsMouseOver(window) && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            OnClick();
        }
    }


    virtual void Update(const sf::RenderWindow& window, const sf::Event& event) {
        bool hovering = IsMouseOver(window);

        HandleClick(window);

        if (hovering != wasHovered) {
            if (hovering) {
                OnHover();
            } else {
                OnHoverExit();
            }
            wasHovered = hovering;
        }


        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            OnRelease(); 
        }
    }

    virtual void OnClick() {
        
        
    }

    virtual void OnHover() {
        
        
    }

    virtual void OnHoverExit() {
        Sprite.setColor(sf::Color::White); 
        
    }
    
    virtual void OnRelease() {
       
    }

protected:
    bool wasHovered = false;                                       
};