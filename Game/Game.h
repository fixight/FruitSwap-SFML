#pragma once
#include <chrono>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../Objects/Object.h"
#include "../UI/UIElementBase.h"
#include "../UI/UIManager.h"


class Game
{
public:
    Game()
    {
        lastTime = std::chrono::high_resolution_clock::now();
    }
    

private:
    std::chrono::high_resolution_clock::time_point lastTime;

public:
    void GameLoop(sf::RenderWindow& PlayerWindow);

    void InitGame();

    void DrawWallper(sf::RenderWindow& PlayerWindow);



    const int MapHeight = 4;

    const int MapWidth = 5;

    Vector2D MapStartPosition = Vector2D(75 , 200);

    UIManager* UIManager;


    std::vector<UIElementBase*> UIElements;

    
    sf::Sprite Background;

    Object* TestObject = nullptr;

    UIElementBase* TestUI = nullptr;
    
};
