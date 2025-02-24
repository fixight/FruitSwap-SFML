#pragma once
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>

#include "FireworkParticle.h"
#include "../Core/Vector2D.h"


class UIElementBase;

class UIManager
{
private:
    static UIManager* instance;
    UIManager();
    ~UIManager() = default; 

    std::vector<UIElementBase*> uiElements;
    std::vector<UIElementBase*> winStars;
    std::vector<Firework*> fireworks;

    Vector2D lastScorePosition;
    Vector2D scoreElementSize;
    Vector2D startPositionBase;
    std::vector<UIElementBase*> Points;
    sf::Font font;

    int levelNumber = 1;


public:
    static UIManager* GetInstance();

    void AddElement(UIElementBase* element);
    void RemoveElement(UIElementBase* element);
    void ArrangeElementsHorizontally(float startX, float startY, float spacing);
    void ArrangeElementsVertically(float startX, float startY, float spacing);
    void DrawUI(sf::RenderWindow& window);
    void UpdateUI(sf::RenderWindow& window, const sf::Event& event);
    void DrawLevelNumber( sf::RenderWindow& window);

    void AddScore();
    void PlayWinAnimation();
    void ClearScoreBoard();
};
