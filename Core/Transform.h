#pragma once
#include "Vector2D.h"

struct Transform
{
public:
    Vector2D Position;
    float Rotation;  
    Vector2D Scale;

        
    Transform()
        : Position(0, 0), Rotation(0), Scale(1, 1) {}
    
    Transform(const Vector2D& position, float rotation, const Vector2D& scale)
        : Position(position), Rotation(rotation), Scale(scale) {}

       
    void SetPosition(const Vector2D& position) {
        Position = position;
    }

    void SetRotation(float rotation) {
        Rotation = rotation;
    }

    void SetScale(const Vector2D& scale) {
        Scale = scale;
    }
};