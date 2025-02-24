#include "GameManager.h"
#include "..//Objects/Cell.h"
#include <iostream> 
#include <cmath> 
#include <algorithm> 

#include "AudioManager.h"

#include "../UI/UIManager.h"


GameManager* GameManager::instance = nullptr;
std::once_flag GameManager::initFlag;


void GameManager::Initialize(int Rows, int Cols)
{
    float elementWidth = 100;
    float elementHeight = 100;
    float margin = 10;

    this->Rows = Rows;
    this->Cols = Cols;



    Board.assign(Cols, std::vector<Cell*>(Rows, nullptr)); // Используем assign для пересоздания Board с новым размером и инициализацией nullptr

    for (int y = 0; y < Cols; ++y)
    {
        for (int x = 0; x < Rows; ++x)
        {
            Cell* el = new Cell();
            el->SetSize(elementWidth, elementHeight);
            el->SetIndex(Vector2D(x, y));

            Vector2D position(
                MapStartPosition.X + (elementWidth + margin) * static_cast<float>(x),
                MapStartPosition.Y + (elementHeight + margin) * static_cast<float>(y)
            );

            el->SetPosition(position);
            el->SetBaseLocation(position);
            Board[y][x] = el;
        }
    }

}

bool GameManager::CanSwap(Cell* TileA, Cell* TileB)
{
    if (!TileA || !TileB) return false;

    Vector2D indexA = TileA->GetIndex();
    Vector2D indexB = TileB->GetIndex();

    // Check if tiles are adjacent
    if (std::abs(indexA.X - indexB.X) + std::abs(indexA.Y - indexB.Y) != 1)
    {

        return false; // Not adjacent, cannot swap
    }

    // Store original positions in board
    Cell* tempA = Board[static_cast<int>(indexA.Y)][static_cast<int>(indexA.X)];
    Cell* tempB = Board[static_cast<int>(indexB.Y)][static_cast<int>(indexB.X)];

    // Simulate swap in Board matrix (only pointers and indices, no visual swap)
    Board[static_cast<int>(indexA.Y)][static_cast<int>(indexA.X)] = TileB;
    Board[static_cast<int>(indexB.Y)][static_cast<int>(indexB.X)] = TileA;
    TileA->SetIndex(indexB);
    TileB->SetIndex(indexA);


    bool canSwap = false;
    if (CheckForMatchAfterSwap(TileA, TileB))
    {
        canSwap = true;
    }

    Board[static_cast<int>(indexA.Y)][static_cast<int>(indexA.X)] = tempA;
    Board[static_cast<int>(indexB.Y)][static_cast<int>(indexB.X)] = tempB;
    TileA->SetIndex(indexA);
    TileB->SetIndex(indexB);


    return canSwap;
}

bool GameManager::CheckForMatchAfterSwap(Cell* TileA, Cell* TileB)
{
    if (!TileA || !TileB) return false;

    Vector2D indexA = TileA->GetIndex();
    Vector2D indexB = TileB->GetIndex();

    if (CheckHorizontalMatch(indexB) || CheckVerticalMatch(indexB) ||
        CheckHorizontalMatch(indexA) || CheckVerticalMatch(indexA))
    {
        return true;
    }
    return false;
}

void GameManager::WinRound()
{
    AudioManager::GetInstance().PlaySound("Sounds/WinRound.wav");
    UIManager::GetInstance()->ClearScoreBoard();
    UIManager::GetInstance()->PlayWinAnimation();

    Score = 0.0f;
    CurrentIndex = 0;
}


bool GameManager::CheckHorizontalMatch(Vector2D index)
{
    int x = static_cast<int>(index.X);
    int y = static_cast<int>(index.Y);
    if (y < 0 || y >= Cols || x < 0 || x >= Rows) return false;
    Cell* cell = Board[y][x];
    if (!cell) return false;

    int matchLength = 1;
    FruitType type = cell->GetCellType();


    for (int i = x - 1; i >= 0; --i)
    {
        if (Board[y][i] && Board[y][i]->GetCellType() == type)
        {
            matchLength++;
        }
        else
        {
            break;
        }
    }

    for (int i = x + 1; i < Rows; ++i)
    {
        if (Board[y][i] && Board[y][i]->GetCellType() == type)
        {
            matchLength++;
        }
        else
        {
            break;
        }
    }

    return matchLength >= 3;
}

bool GameManager::CheckVerticalMatch(Vector2D index)
{
    int x = static_cast<int>(index.X);
    int y = static_cast<int>(index.Y);
    if (y < 0 || y >= Cols || x < 0 || x >= Rows) return false;
    Cell* cell = Board[y][x];
    if (!cell) return false;

    int matchLength = 1;
    FruitType type = cell->GetCellType();


    for (int i = y - 1; i >= 0; --i)
    {
        if (Board[i][x] && Board[i][x]->GetCellType() == type)
        {
            matchLength++;
        }
        else
        {
            break;
        }
    }

    for (int i = y + 1; i < Cols; ++i)
    {
        if (Board[i][x] && Board[i][x]->GetCellType() == type)
        {
            matchLength++;
        }
        else
        {
            break;
        }
    }
    return matchLength >= 3;
}


void GameManager::SwapTiles(Cell* TileA, Cell* TileB)
{
    if (!TileA || !TileB) return;

    Vector2D indexA = TileA->GetIndex();
    Vector2D indexB = TileB->GetIndex();


    Vector2D baseLocationA = TileA->GetBaseLocation();
    Vector2D baseLocationB = TileB->GetBaseLocation();


    Board[static_cast<int>(indexA.Y)][static_cast<int>(indexA.X)] = TileB;
    Board[static_cast<int>(indexB.Y)][static_cast<int>(indexB.X)] = TileA;
    
    TileA->SetIndex(indexB);
    TileB->SetIndex(indexA);


    TileA->SetPosition(baseLocationB);
    TileB->SetPosition(baseLocationA);


    TileA->SetBaseLocation(baseLocationB);
    TileB->SetBaseLocation(baseLocationA);



    TryToMatchCells(); 
}

void GameManager::Update(float DeltaTime , sf::RenderWindow& window , sf::Event& event)
{
    for (const auto& row : GameManager::GetInstance().GetBoard()) 
    {
        for (Cell* element : row) 
        {
            if (element) 
            {
                element->Draw(window);
                element->Update(window , event);
            }
        }
    }

}

void GameManager::CheckMatches()
{
    std::vector<Cell*> matchedCells;
    bool foundMatch = false;
    bool playedMatchSound = false; 

    for (int y = 0; y < Cols; ++y)
    {
        for (int x = 0; x < Rows - 2; ++x)
        {
            Cell* cell1 = Board[y][x];
            if (!cell1) continue; 


            std::vector<Cell*> currentMatch;
            currentMatch.push_back(cell1);

            for (int offset = 1; x + offset < Rows; ++offset)
            {
                Cell* nextCell = Board[y][x + offset];
                if (nextCell && nextCell->GetCellType() == cell1->GetCellType())
                {
                    currentMatch.push_back(nextCell);
                } else {
                    break;
                }
            }

            if (currentMatch.size() >= 3)
            {
                foundMatch = true;
                for (Cell* cell : currentMatch)
                {
                    std::cout << "(" << cell->GetIndex().X << "," << cell->GetIndex().Y << ") ";
                    matchedCells.insert(matchedCells.end(), currentMatch.begin(), currentMatch.end()); // Add all cells in the match
                }
                std::cout << std::endl;
                x += currentMatch.size() - 1;
            }
        }
    }

    for (int x = 0; x < Rows; ++x)
    {
        for (int y = 0; y < Cols - 2; ++y)
        {
            Cell* cell1 = Board[y][x];
            if (!cell1) continue; // Check if cell1 is not nullptr

            std::vector<Cell*> currentMatch;
            currentMatch.push_back(cell1);

            for (int offset = 1; y + offset < Cols; ++offset)
            {
                Cell* nextCell = Board[y + offset][x];
                if (nextCell && nextCell->GetCellType() == cell1->GetCellType())
                {
                    currentMatch.push_back(nextCell);
                } else {
                    break;
                }
            }

            if (currentMatch.size() >= 3)
            {
                foundMatch = true;

                for (Cell* cell : currentMatch)
                {

                    matchedCells.insert(matchedCells.end(), currentMatch.begin(), currentMatch.end()); // Add all cells in the match
                }
                std::cout << std::endl;
                y += currentMatch.size() - 1;
            }
        }
    }
    
    std::sort(matchedCells.begin(), matchedCells.end());
    matchedCells.erase(std::unique(matchedCells.begin(), matchedCells.end()), matchedCells.end());


    std::vector<int> removedCellCountPerColumn(Rows, 0);

    if (foundMatch)
    {
        if (!playedMatchSound) 
        {
            AudioManager::GetInstance().PlaySound("Sounds/OnMatch.wav");
            playedMatchSound = true; 
        }


        for (Cell* cell : matchedCells)
        {
            std::cout << static_cast<int>(cell->GetCellType()) << " ";
        }
        std::cout << std::endl;
        NotifyAll(matchedCells.size());
        
        float scoreToAdd = matchedCells.size() * 100.0f;
        AddScore(scoreToAdd);
       


        for (Cell* cellToRemove : matchedCells)
        {
            if (cellToRemove)
            {

                Vector2D index = cellToRemove->GetIndex();
                int y = static_cast<int>(index.Y);
                int x = static_cast<int>(index.X);


                Board[y][x] = nullptr;
                removedCellCountPerColumn[x]++;


            }
        }
        
        ApplyGravity();
        SpawnNewCells(removedCellCountPerColumn);
        TryToMatchCells(); 
    }
}

void GameManager::SpawnNewCells(const std::vector<int>& removedCellCountPerColumn)
{
    float elementWidth = 100;
    float elementHeight = 100;
    float margin = 10;

    for(int x = 0; x < removedCellCountPerColumn.size(); ++x)
    {
        for(int i = 0; i < removedCellCountPerColumn[x]; ++i)
        {
            Cell* cell = new Cell();
            cell->SetSize(elementWidth, elementHeight);

            int spawnYIndex = -1;

            for (int y = 0; y < Cols; ++y)
            {
                if (Board[y][x] == nullptr)
                {
                    spawnYIndex = y;
                    break;
                }
            }
            if (spawnYIndex == -1)
            {
                spawnYIndex = 0;
            }


            Vector2D spawnPosition(
                MapStartPosition.X + (elementWidth + margin) * static_cast<float>(x),
                MapStartPosition.Y - elementHeight
            );

            cell->SetPosition(spawnPosition);

            Vector2D targetPosition(
                MapStartPosition.X + (elementWidth + margin) * static_cast<float>(x),
                MapStartPosition.Y + (elementHeight + margin) * static_cast<float>(spawnYIndex)
            );

            cell->SetFallingTo(targetPosition);
            cell->SetBaseLocation(targetPosition);
            cell->SetIndex(Vector2D(x, spawnYIndex));

            Board[spawnYIndex][x] = cell;

            

        }
    }
}

void GameManager::TryToMatchCells()
{
    for (const auto& row : Board)
    {
        for (Cell* element : row)
        {
            if (element != nullptr)
            {
                if(element->GetIsFalling()) return;
            }
        }
    }
    CheckMatches(); 
}


void GameManager::ApplyGravity()
{
    float elementWidth = 100;
    float elementHeight = 100;
    float margin = 10;


    bool somethingMoved = true;
    int gravityPass = 0;
    while (somethingMoved)
    {
        somethingMoved = false;
        gravityPass++;
        

        for (int x = 0; x < Rows; ++x)
        {
            for (int y = Cols - 2; y >= 0; --y)
            {
                if (Board[y][x] != nullptr)
                {
                    Cell* cellToMove = Board[y][x];
                    int emptySpaceBelow = 0;
                    for (int k = y + 1; k < Cols; ++k)
                    {
                        if (Board[k][x] == nullptr)
                        {
                            emptySpaceBelow++;
                        }

                    }

                    if (emptySpaceBelow > 0)
                    {
                        
                        Board[y][x] = nullptr;
                        int newY = y + emptySpaceBelow;
                        
                        Board[newY][x] = cellToMove;

                        Vector2D oldIndex = cellToMove->GetIndex();
                        Vector2D newIndex = Vector2D(oldIndex.X, newY);
                        cellToMove->SetIndex(newIndex);

                        Vector2D newPosition(
                            MapStartPosition.X + (elementWidth + margin) * static_cast<float>(newIndex.X),
                            MapStartPosition.Y + (elementHeight + margin) * static_cast<float>(newIndex.Y)
                        );
                        cellToMove->SetFallingTo(newPosition);
                        cellToMove->SetBaseLocation(newPosition);
                        somethingMoved = true;

                    
                    } 
                    
                }
            }
        }
        std::cout << "  Gravity Pass: " << gravityPass << " finished, somethingMoved: " << somethingMoved << std::endl;
         if (gravityPass > Cols) {
             std::cerr << "Warning: ApplyGravity reached pass limit, possible infinite loop!" << std::endl;
             break;
         }
    }
    std::cout << "--- ApplyGravity() finished ---" << std::endl;
}


Cell* GameManager::GetNeighborCell(Cell* cell, const Vector2D& direction)
{
    Vector2D cellIndex = cell->GetIndex();
    Vector2D normalizedDirection = Vector2D(0, 0);

    if (direction.Length() == 0)
    {
        return nullptr;
    }

    if (std::abs(direction.X) > std::abs(direction.Y))
    {
        normalizedDirection.X = (direction.X > 0) ? 1 : -1;
        normalizedDirection.Y = 0;
    }
    else
    {
        normalizedDirection.Y = (direction.Y > 0) ? 1 : -1;
        normalizedDirection.X = 0;
    }

    Vector2D neighborIndex = cellIndex + normalizedDirection;

    if (neighborIndex.X < 0 || neighborIndex.X >= Rows || neighborIndex.Y < 0 || neighborIndex.Y >= Cols)
    {
        return nullptr;
    }

    return Board[static_cast<int>(neighborIndex.Y)][static_cast<int>(neighborIndex.X)];
}



void GameManager::AddScore(float Count)
{
    Score += Count / 3.0f;

    std::cout<<Score / MaxScore * 10.0f << "Score" << std::endl;

    if(Score / MaxScore * 10.0f > CurrentIndex)
    {
        UIManager::GetInstance()->AddScore();
        CurrentIndex++;

        if(CurrentIndex == 8)
        {
            WinRound();
        }
    }



    
}


bool GameManager::GetHint()
{

    for (int y = 0; y < Cols; ++y)
    {
        for (int x = 0; x < Rows; ++x)
        {
            Cell* currentCell = Board[y][x];
            if (!currentCell) continue;
            if (currentCell->GetIsMoving() || currentCell->GetIsHintMoving()) continue;


            std::vector<Vector2D> directions = {
                Vector2D(1, 0),
                Vector2D(-1, 0),
                Vector2D(0, 1),
                Vector2D(0, -1)
            };

            for (const auto& dir : directions)
            {
                Vector2D neighborIndex = currentCell->GetIndex() + dir;
                if (neighborIndex.X < 0 || neighborIndex.X >= Rows ||
                    neighborIndex.Y < 0 || neighborIndex.Y >= Cols)
                    continue;

                Cell* neighborCell = Board[static_cast<int>(neighborIndex.Y)][static_cast<int>(neighborIndex.X)];
                if (!neighborCell) continue;
                if (neighborCell->GetIsMoving() || neighborCell->GetIsHintMoving()) continue;


                if (CanSwap(currentCell, neighborCell))
                {
                    currentCell->MarkCell(sf::Color::Cyan, true);
                    neighborCell->MarkCell(sf::Color::Cyan, true);

                    Vector2D neighborPos = neighborCell->GetPosition();
                    Vector2D currentPos = currentCell->GetPosition();

                    currentCell->SetMoveToAndBack(neighborPos);
                    neighborCell->SetMoveToAndBack(currentPos);

                    std::cout << "Hint: Swap (" << currentCell->GetIndex().X << "," << currentCell->GetIndex().Y
                              << ") with (" << neighborCell->GetIndex().X << "," << neighborCell->GetIndex().Y << ")\n";
                    return true;
                }
            }
        }
    }

    std::cout << "Hint: No possible swaps found.\n";
    return false;
}