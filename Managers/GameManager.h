#pragma once

#include <vector>
#include <mutex> // Добавляем для потокобезопасности
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "../Core/Vector2D.h"
#include "../Interfaces/Interfaces.h"


class Object;
class Cell;

class GameManager {
public:
    static GameManager& GetInstance() {
        std::call_once(initFlag, [&]() {
            instance = new GameManager();
        });
        return *instance;
    }

    void Initialize(int Rows, int Cols);
    bool CanSwap(Cell* TileA, Cell* TileB);
    void SwapTiles(Cell* TileA, Cell* TileB);
    void Update(float DeltaTime , sf::RenderWindow& window , sf::Event& event);
    bool GetHint();
    void CheckMatches();
    void ApplyGravity();
    void SpawnNewCells(const std::vector<int>& removedCellCountPerColumn);
    void TryToMatchCells();
    bool CheckHorizontalMatch(Vector2D index);
    bool CheckVerticalMatch(Vector2D index);
    bool CheckForMatchAfterSwap(Cell* TileA, Cell* TileB);
    void WinRound();

    Cell* GetNeighborCell(Cell* cell, const Vector2D& direction);

    Vector2D GetMapSize(){return Vector2D(Rows, Cols);}

    const std::vector<std::vector<Cell*>>& GetBoard() const { return Board; }

    bool GetIsDraggingCell() const {
        return isDraggingCell;
    }

    void SetIsDraggingCell(bool value) {
        isDraggingCell = value;
    }

    void PlayWinAnimation();

    void AddScore(float Count);
    float GetScore() const { return Score; } 

    void NotifyAll(float Power)
    {
        for (IMatchHandler* element : matchHandlers)
        {
            element->Notify(Power);
        }
    }

    Vector2D GetMapSize() const { return Vector2D(Rows, Cols); }



protected:


private:
    std::vector<std::vector<Cell*>> Board;
    int Rows = 5;
    int Cols = 4;
    int MaxCols = 9; 
    Vector2D MapStartPosition = Vector2D(80 , 300);

    std::vector<IMatchHandler*> matchHandlers;
    

    float Score = 0.0f;
    float MaxScore = 9000.0f;

    int CurrentIndex = 0;

    bool isDraggingCell = false;

    static GameManager* instance;
    static std::once_flag initFlag;

};