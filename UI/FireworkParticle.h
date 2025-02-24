#pragma once

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "../Core/Vector2D.h"
#include "../Objects/Object.h"




class FireworkParticle : public Object {
public:
    FireworkParticle(const Vector2D& position)
        : Object("textures/star.png"), velocity(RandomVelocity()), lifetime(3.0f) {
        SetPosition(position);
        SetScale(Vector2D(1.1f, 1.1f));
    }

    void Update() {
        float deltaTime = 1.0f / 120.0f;
        Move(velocity * deltaTime);
        lifetime -= deltaTime;
        if (lifetime <= 0.0f) {
            SetIsVisible(false);
        }
    }
    Vector2D RandomVelocity() {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<float> dist(-100.0f, 100.0f);
        return Vector2D(dist(rng), dist(rng));
    }

    bool IsAlive() const { return lifetime > 0.0f; }

private:

    Vector2D velocity;
    float lifetime;
};

class Firework {
public:
    Firework(const Vector2D& position) {
        for (int i = 0; i < 50; ++i) {
            particles.push_back(new FireworkParticle(position));
        }
    }

    ~Firework() {
        for (auto particle : particles) {
            delete particle;
        }
    }

    void Update(float deltaTime) {
        for (auto particle : particles) {
            if (particle->IsAlive()) {
                particle->Update();
            }
        }
    }

    void Draw(sf::RenderWindow& window) {
        for (auto particle : particles) {
            if (particle->IsAlive()) {
                particle->Draw(window);
            }
        }
    }

private:
    std::vector<FireworkParticle*> particles;
};
