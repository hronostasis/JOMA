#pragma once
#include <SFML/Graphics.hpp>

class Animation {
public:
    Animation(sf::Texture& texture, int columns, int rows, float frameDuration, bool loop = true);
    void update(float deltaTime);
    void applyTo(sf::Sprite& sprite);
    void reset();
    bool isFinished() const { return finished; }

private:
    sf::Vector2i frameSize;
    int columns, frameCount;
    float frameDuration;
    bool loop;
    float timer = 0.f;
    int currentFrame = 0;
    bool finished = false;
};