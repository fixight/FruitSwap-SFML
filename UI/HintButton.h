#pragma once
#include "UIElementBase.h"
#include "../Managers/GameManager.h"

class HintButton : public UIElementBase
{
public:
    HintButton(const std::string& path) : UIElementBase(path)
    {
        
    }

    void OnHover() override
    {
        if(HintsLeft > 0)
            Sprite.setColor(sf::Color(150, 200, 255)); 
    }

    void OnHoverExit() override
    {
        if(HintsLeft > 0)
            Sprite.setColor(sf::Color::White); 
    }

    void OnClick() override
    {
        if(HintsLeft > 0)
            GameManager::GetInstance().GetHint();

        HintsLeft--;
        if (HintsLeft < 0)
        {
            Sprite.setColor(sf::Color(128, 128, 128, 200)); 

        }
        
    }

private:
    int HintsLeft = 2;
};
