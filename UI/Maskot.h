#pragma once
#include "UIElementBase.h"
#include "../Interfaces/Interfaces.h"


class Maskot : public UIElementBase 
{
public:
    Maskot(const std::string& pth) : UIElementBase(pth)
    {
        
    }

    void Update(const sf::RenderWindow& window, const sf::Event& event) override;

private:
    float startY = 0.0f;   
    float time = 0.0f;     
    float amplitude = 6.f; 
    float speed = 1.3f;     
};

