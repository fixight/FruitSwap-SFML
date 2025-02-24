#include "Game.h"

#include <random>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "../Managers/AudioManager.h"
#include "../Objects/Cell.h"

#include "../Managers/TextureManager.h"
#include "../Objects/Object.h"

class Cell;
enum class FruitType;

void Game::GameLoop(sf::RenderWindow& PlayerWindow)
{
    while (PlayerWindow.isOpen())
    {
        sf::Event event;
        while (PlayerWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                PlayerWindow.close();
            }
        }

        PlayerWindow.clear();

        DrawWallper(PlayerWindow);

        auto currentTime = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> deltaTime = currentTime - lastTime;

        const auto deltaTimeS = deltaTime.count();


        lastTime = currentTime;

        UIManager::GetInstance()->DrawUI(PlayerWindow);
        UIManager::GetInstance()->UpdateUI(PlayerWindow , event);
        

        GameManager::GetInstance().Update(deltaTimeS , PlayerWindow , event);

        PlayerWindow.display();
    }
}


void Game::InitGame()
{

    AudioManager::GetInstance().SetMusic("Sounds/MainTheme.mp3" , true , 20);
    sf::Texture texture;
    if (!texture.loadFromFile("Textures/fon.png")) {
        std::cerr << "Ошибка загрузки фона!" << std::endl;
        return;
    }
    Background.setTexture(texture);

    sf::RenderWindow window(sf::VideoMode(600, 900), "SFML Test");
    window.setFramerateLimit(120); 

    sf::Vector2u textureSize = texture.getSize();

    sf::Vector2u windowSize = window.getSize();

    float scaleX = (float)windowSize.x / textureSize.x;
    float scaleY = (float)windowSize.y / textureSize.y;

    Background.setScale(scaleX, scaleY);

    TestUI = new UIElementBase("Textures/avokado.png");
    Background.setTexture(texture);
    TestObject = new Object("Textures/avokado.png");
    


    TestUI->SetSize(50, 50); 

    GameManager::GetInstance().Initialize(4 , 5);

    
    
    GameLoop(window);

}

void Game::DrawWallper(sf::RenderWindow& PlayerWindow)
{
    PlayerWindow.draw(Background);
}

