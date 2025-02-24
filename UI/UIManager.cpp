#include "UIManager.h"
#include <random>

#include "FireworkParticle.h"
#include "HintButton.h"
#include "Maskot.h"

UIManager* UIManager::instance = nullptr;

UIManager* UIManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new UIManager();
    }
    return instance;
}

UIManager::UIManager() : lastScorePosition(0, 0), scoreElementSize(0, 0) 
{
    if (!font.loadFromFile("Fonts/Oi-Regular.ttf")) { 
        std::cerr << "Ошибка: не удалось загрузить шрифт!" << std::endl;
    }
    UIElementBase* Uroven  = new UIElementBase("textures/uroven.png");
    Uroven->SetPosition(Vector2D(160, 180));
    Uroven->SetScale(Vector2D(1.3f, 1.3f));
    AddElement(Uroven);

    Maskot* Maskot1 = new Maskot("textures/mascot01.png");
    Maskot1->SetPosition(Vector2D(80, 140));
    Maskot1->SetScale(Vector2D(0.41f, 0.41f));
    AddElement(Maskot1);

    Maskot* Maskot2 = new Maskot("textures/mascot2.png");
    Maskot2->SetPosition(Vector2D(400, 165));
    Maskot2->SetScale(Vector2D(0.43f, 0.43f));
    AddElement(Maskot2);

    UIElementBase* LVL = new UIElementBase("textures/lvlnull.png");
    LVL->SetPosition(Vector2D(43, 30));
    LVL->SetScale(Vector2D(1.2f, 1.2f));
    AddElement(LVL);

    UIElementBase* Settings = new UIElementBase("textures/settings.png");
    Settings->SetPosition(Vector2D(20, 815));
    AddElement(Settings);

    HintButton* Key = new HintButton("textures/key.png");
    Key->SetPosition(Vector2D(520, 815));
    AddElement(Key);

    UIElementBase* Status = new UIElementBase("textures/statuds.png");
    Status->SetPosition(Vector2D(105, 20));
    Status->SetScale(Vector2D(1.8f, 1.8));
    AddElement(Status);

    UIElementBase* Score = new UIElementBase("textures/Point.png");
    Score->SetPosition(Vector2D(141, 100));
    Score->SetScale(Vector2D(2.2f, 2.4f));
    AddElement(Score);

    Score->SetIsVisible(false);
    lastScorePosition = Score->GetPosition();
    scoreElementSize = Score->GetSize();
    lastScorePosition.X -= scoreElementSize.X;
    startPositionBase = lastScorePosition;
}

void UIManager::AddElement(UIElementBase* element)
{
    if (element)
    {
        uiElements.push_back(element);
    }
}

void UIManager::RemoveElement(UIElementBase* element)
{
    for (auto it = uiElements.begin(); it != uiElements.end(); ++it)
    {
        if (*it == element)
        {
            delete *it; 
            uiElements.erase(it);
            return; 
        }
    }
}

void UIManager::ArrangeElementsHorizontally(float startX, float startY, float spacing)
{
    float currentX = startX;
    for (UIElementBase* element : uiElements)
    {
        if (element)
        {
            element->SetPosition(Vector2D(currentX, startY));
            currentX += element->GetSize().X + spacing;
        }
    }
}

void UIManager::ArrangeElementsVertically(float startX, float startY, float spacing)
{
    float currentY = startY;
    for (UIElementBase* element : uiElements)
    {
        if (element)
        {
            element->SetPosition(Vector2D(startX, currentY));
            currentY += element->GetSize().Y + spacing; 
        }
    }
}

void UIManager::DrawUI(sf::RenderWindow& window)
{
    for (UIElementBase* element : uiElements)
    {
        if (element)
        {
            element->Draw(window);
        }
    }
    
    for (auto& firework : fireworks)
    {
        firework->Draw(window);
    }
}

void UIManager::UpdateUI(sf::RenderWindow& window, const sf::Event& event)
{
    for (UIElementBase* element : uiElements)
    {
        if (element)
        {
            element->Update(window, event);
        }
    }


    for (auto firework : fireworks)
    {
        firework->Update(0.016f); 
    }

    DrawLevelNumber(window);


    
}

void UIManager::DrawLevelNumber(sf::RenderWindow& window) {
    sf::Text levelText;
    levelText.setFont(font); 
    levelText.setString(std::to_string(levelNumber)); 
    levelText.setCharacterSize(50); 
    levelText.setFillColor(sf::Color::White); 
    levelText.setPosition(335, 207); 

    window.draw(levelText);
}

void UIManager::AddScore()
{
    UIElementBase* scorePoint = new UIElementBase("textures/Point.png");
    scorePoint->SetScale(Vector2D(2.2f, 2.4f)); 

    lastScorePosition.X += scoreElementSize.X ; 
    scorePoint->SetPosition(lastScorePosition);
    AddElement(scorePoint);
    Points.push_back(scorePoint);
    scoreElementSize = scorePoint->GetSize(); 
}

void UIManager::PlayWinAnimation()
{
    levelNumber++;
    Firework* firework = new Firework(Vector2D(300, 300)); 
    fireworks.push_back(firework);
}

void UIManager::ClearScoreBoard()
{
    for (auto element : Points)
    {
        RemoveElement(element);
    }

    lastScorePosition = startPositionBase;
}
