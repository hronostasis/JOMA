#pragma once
#include <SFML/Graphics.hpp>

class Animation {
public:
    Animation(sf::Texture& texture, int columns, int rows, float frameDuration);
    void update(float deltaTime);
    void applyTo(sf::Sprite& sprite);

private:
    sf::Vector2i frameSize;
    int columns, frameCount;
    float frameDuration;
    float timer = 0.f;
    int currentFrame = 0;
};