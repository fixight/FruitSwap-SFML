#include "Maskot.h"

void Maskot::Update(const sf::RenderWindow& window, const sf::Event& event)
{
    if (startY == 0.0f) {
        startY = GetPosition().Y; // Запоминаем начальную Y-позицию
    }

    time += speed * 0.008f; // Увеличиваем время (предполагая 60 FPS)

    float offsetY = std::sin(time) * amplitude; // Вычисляем смещение по синусоиде
    SetPosition(Vector2D(GetPosition().X, startY + offsetY));
    std::cout << "Maskot Y: " <<offsetY << std::endl;

}
