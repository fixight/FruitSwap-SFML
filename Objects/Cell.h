// Cell.h (modified)
#pragma once

#include <random>
#include "../UI/UIElementBase.h"
#include "../Managers/TextureManager.h"
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <iostream>
#include <SFML/Graphics/Color.hpp>

#include "../Managers/AudioManager.h"

enum class FruitType;

enum CellRestrictionFlags
{
    NONE  = 0,
    BLOCK_UP = 1 << 0,   // 0001
    BLOCK_DOWN = 1 << 1, // 0010
    BLOCK_LEFT = 1 << 2, // 0100
    BLOCK_RIGHT = 1 << 3 // 1000
};

#include "../Managers/GameManager.h"


class Cell : public UIElementBase
{
public:
    Cell() : UIElementBase(TextureManager::GetInstance().GetTexture(SetRandomFruitType()))
    {
        originalColor = Sprite.getColor(); 
        shakeAmplitude = 3.0f;
        shakeFrequency = 13.0f;
        highlightDuration = 1.0f; 
        moveDuration = 0.15f; 
    }

    void OnClick() override
    {
        if (IsFalling || isHighlighted || IsMoving || IsHintMoving) return; 
        if (GameManager::GetInstance().GetIsDraggingCell()) return; 

        IsDraging = true;
        GameManager::GetInstance().SetIsDraggingCell(true); 
        sf::Vector2i mousePosition = sf::Mouse::getPosition();
        BasePosition = Position;
        DragOffset = Position - Vector2D(mousePosition.x, mousePosition.y);
        InitialDragDirection = Vector2D(0, 0);  
    }

    void Update(const sf::RenderWindow& window, const sf::Event& event) override
    {
        UIElementBase::Update(window, event);

        float deltaTime = 1.0f/120.0f; 

        if (isHighlighted)
        {
            UpdateHighlightEffect(deltaTime);
            return;
        }


        if(IsFalling)
        {
            Fall();
            return;
        }

        if(IsMoving || IsHintMoving) 
        {
            MoveTo(deltaTime);
            return;
        }

        if (IsDraging)
        {
            sf::Vector2i mousePosition = sf::Mouse::getPosition();
            Vector2D newPosition = Vector2D(mousePosition.x, mousePosition.y) + DragOffset;
            Vector2D offset = newPosition - BasePosition;

            constexpr float SnapThreshold = 9.0f;
            constexpr float DirectionSwitchThreshold = 13.0f;
            constexpr float SwapThreshold = 50.0f; 


            if (InitialDragDirection.X == 0 && InitialDragDirection.Y == 0)
            {
                if (std::abs(offset.X) > std::abs(offset.Y) && std::abs(offset.X) > 2)
                {
                    InitialDragDirection = Vector2D(1, 0); 
                }
                else if (std::abs(offset.Y) > 2)
                {
                    InitialDragDirection = Vector2D(0, 1); 
                }
            }
            else 
            {
                if (InitialDragDirection.X == 1) 
                {
                    if (std::abs(offset.X) < DirectionSwitchThreshold && std::abs(offset.Y) > std::abs(offset.X) * 1.5f && std::abs(offset.Y) > 5.0f)
                    {
                        InitialDragDirection = Vector2D(0, 1); 
                    }
                }
                else if (InitialDragDirection.Y == 1) 
                {
                    if (std::abs(offset.Y) < DirectionSwitchThreshold && std::abs(offset.X) > std::abs(offset.Y) * 1.5f && std::abs(offset.X) > 5.0f)
                    {
                        InitialDragDirection = Vector2D(1, 0); 
                    }
                }
            }

            if (InitialDragDirection.X == 1)
            {
                newPosition.Y = BasePosition.Y;
                if (Restrictions & BLOCK_LEFT && offset.X < 0.0f) {
                    newPosition.X = BasePosition.X;
                }
                if (Restrictions & BLOCK_RIGHT && offset.X > 0.0f) {
                    newPosition.X = BasePosition.X;
                }
            }
            else if (InitialDragDirection.Y == 1)
            {
                newPosition.X = BasePosition.X;
                if (Restrictions & BLOCK_UP && offset.Y < 0.0f) {
                    newPosition.Y = BasePosition.Y;
                }
                if (Restrictions & BLOCK_DOWN && offset.Y > 0.0f) {
                    newPosition.Y = BasePosition.Y;
                }
            }

            SetPosition(newPosition);

            Cell* cell = GameManager::GetInstance().GetNeighborCell(this, newPosition - BasePosition);

            if(cell != nullptr)
            {
                Vector2D neighborOffset = cell->GetPosition() - cell->GetBaseLocation();
                float  effectiveOffsetLength = (InitialDragDirection.X == 1) ? std::abs(offset.X) : std::abs(offset.Y);
                
                if (effectiveOffsetLength >= SwapThreshold)
                {
                    if (GameManager::GetInstance().CanSwap(this, cell))
                    {
                        GameManager::GetInstance().SwapTiles(this, cell);
                        IsDraging = false;
                        GameManager::GetInstance().SetIsDraggingCell(false);
                        SetPosition(BasePosition);
                        cell->SetPosition(cell->GetBaseLocation());
                    }
                    else
                    {
                        SetPosition(BasePosition);
                        IsDraging = false;
                        GameManager::GetInstance().SetIsDraggingCell(false);
                        MarkCell(sf::Color::Red, true);
                        cell->MarkCell(sf::Color::Red, true);
                        AudioManager::GetInstance().PlaySound("Sounds/Error.wav");
                    }
                }
                else
                {
                    float moveFactor = 0.0f;
                    if (effectiveOffsetLength > 0)
                    {
                        moveFactor = std::min(effectiveOffsetLength / SwapThreshold, 1.0f);
                    }

                    Vector2D neighborNewPosition = cell->GetBaseLocation();
                    if (InitialDragDirection.X == 1)
                    {
                        neighborNewPosition.X -= offset.X * moveFactor;
                    }
                    else if (InitialDragDirection.Y == 1)
                    {
                        neighborNewPosition.Y -= offset.Y * moveFactor;
                    }
                    cell->SetPosition(neighborNewPosition);
                }
            }
        }
    }

    void SetFallingTo(const Vector2D& position)
    {
        IsFalling = true;
        FallDownPosition = position;
    }

    void Fall()
    {

        if(Position.Y > FallDownPosition.Y)
        {
            IsFalling = false;
            SetPosition(FallDownPosition);
            GameManager::GetInstance().TryToMatchCells();
            return;
        }

        Move(Vector2D(0, 4.f));
    }


    void OnRelease() override
    {
        if(IsFalling || isHighlighted || IsMoving || IsHintMoving) return; 
        SetPosition(BasePosition);
        IsDraging = false;
        GameManager::GetInstance().SetIsDraggingCell(false); 
    }

    void SetIndex(const Vector2D& index)
    {
        CellIndex = index;

        Restrictions = 0;

        if (CellIndex.X == 0) Restrictions |= BLOCK_LEFT;
        if (CellIndex.X == GameManager::GetInstance().GetMapSize().X - 1) Restrictions |= BLOCK_RIGHT;
        if (CellIndex.Y == 0) Restrictions |= BLOCK_UP;
        if (CellIndex.Y == GameManager::GetInstance().GetMapSize().Y - 1) Restrictions |= BLOCK_DOWN;
    }

    Vector2D GetIndex() const
    {
        return CellIndex;
    }

    FruitType SetRandomFruitType()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> distrib(0, 2);
        FruitType randomFruit = static_cast<FruitType>(distrib(gen));
        type = randomFruit;
        return randomFruit;
    }

    void SetBaseLocation(Vector2D baselocation)
    {
        BasePosition = baselocation;
    }

    void SetMoveTo(const Vector2D& position, bool isHint = false)
    {
        if((IsMoving || IsHintMoving) && !isHint) return; 

        if(isHint) IsHintMoving = true;
        else IsMoving = true;

        TargetPosition = position;
        moveTimer = 0.0f;
        startPositionForMove = Position; 
        isMovingBack = false;
    }

    void SetMoveToAndBack(const Vector2D& position)
    {
        if(IsMoving || IsHintMoving) return; 
        IsHintMoving = true;
        TargetPosition = position;
        moveTimer = 0.0f;
        startPositionForMove = Position;
        isMovingBack = false; 
    }

    void OnHover() override
    {
        Sprite.setColor(sf::Color(150, 200, 255)); 
    }

    void OnHoverExit() override
    {
        Sprite.setColor(sf::Color::White); 
    }



    void MoveTo(float deltaTime)
    {
        moveTimer += deltaTime;
        float progress = moveTimer / moveDuration; // Calculate progress as a fraction of move duration
        if (progress >= 1.0f)
        {
            progress = 1.0f; // Clamp progress to 1 to avoid overshoot

            if (!isMovingBack && IsHintMoving) // If we are doing a hint move and not moving back yet
            {
                isMovingBack = true; // Set flag to move back
                TargetPosition = BasePosition; // Target is now the original position
                startPositionForMove = Position; // Start from current (neighbor's) position
                moveTimer = 0.0f; // Reset timer for move back
                return; // Start moving back in the next frame
            }
            else
            {
                IsMoving = false; // Stop general movement
                IsHintMoving = false; // Stop hint movement
            }

        }

        // Linear interpolation for smooth movement
        Vector2D currentPosition;
        currentPosition.X = startPositionForMove.X + (TargetPosition.X - startPositionForMove.X) * progress;
        currentPosition.Y = startPositionForMove.Y + (TargetPosition.Y - startPositionForMove.Y) * progress;
        SetPosition(currentPosition);

        if (!IsMoving && !IsHintMoving) // Ensure final position is exactly the target after both moves
        {
             SetPosition(TargetPosition); // Should be BasePosition after move back in hint case
        }
    }


    void MarkCell(sf::Color highlightColor, bool useShakeEffect = false)
    {
        if (isHighlighted) return;
        isHighlighted = true;
        SetPosition(BasePosition);
        Sprite.setColor(highlightColor);
        highlightTimer = 0.0f;
        originalPositionForHighlight = BasePosition;
        this->useShake = useShakeEffect;
        this->highlightingColor = highlightColor;
    }

    Vector2D GetBaseLocation() const {return BasePosition;}

    FruitType GetCellType() const { return type; }
    bool GetIsDraging() const { return IsDraging; }
    bool GetIsFalling() const { return IsFalling; }
    bool GetIsHighlighted() const { return isHighlighted; }
    bool GetIsMoving() const { return IsMoving; }
    bool GetIsHintMoving() const { return IsHintMoving; }

private:
    void UpdateHighlightEffect(float deltaTime)
    {
        highlightTimer += deltaTime;
        if (highlightTimer >= highlightDuration)
        {
            isHighlighted = false;
            Sprite.setColor(originalColor);
            SetPosition(originalPositionForHighlight);
            return;
        }

        // Color lerp back to original
        float colorProgress = highlightTimer / highlightDuration;
        sf::Color currentColor;
        currentColor.r = highlightingColor.r + (originalColor.r - highlightingColor.r) * colorProgress;
        currentColor.g = highlightingColor.g + (originalColor.g - highlightingColor.g) * colorProgress;
        currentColor.b = highlightingColor.b + (originalColor.b - highlightingColor.b) * colorProgress;
        currentColor.a = highlightingColor.a + (originalColor.a - highlightingColor.a) * colorProgress;
        Sprite.setColor(currentColor);

        if (useShake)
        {
            float shakeOffsetX = shakeAmplitude * std::sin(highlightTimer * shakeFrequency);
            float shakeOffsetY = shakeAmplitude * std::cos(highlightTimer * shakeFrequency * 0.7f);
            SetPosition(originalPositionForHighlight + Vector2D(shakeOffsetX, shakeOffsetY));
        }
    }


    Vector2D CellIndex = Vector2D(0, 0);
    bool IsDraging = false;
    bool IsFalling = false;
    bool IsMoving = false;
    bool IsHintMoving = false; 
    bool isMovingBack = false; 
    bool isHighlighted = false;
    Vector2D FallDownPosition;
    Vector2D DragOffset;
    Vector2D InitialDragDirection;
    Vector2D BasePosition;
    Vector2D TargetPosition; 
    Vector2D startPositionForMove; 
    int Restrictions = 0;
    float Alpha = 0.0f;
    FruitType type;

    sf::Color originalColor;
    sf::Color highlightingColor;
    float highlightTimer;
    float highlightDuration;
    float shakeAmplitude;
    float shakeFrequency;
    Vector2D originalPositionForHighlight;
    bool useShake;
    float moveTimer;
    float moveDuration; // Duration of the move in seconds
};