#include "Animation.h"

Animation::Animation(sf::Texture& texture, int columns, int rows, float frameDuration)
    : columns(columns), frameDuration(frameDuration)
{
    sf::Vector2u texSize = texture.getSize();
    frameSize = { (int)(texSize.x / columns), (int)(texSize.y / rows) };
    frameCount = columns * rows;
}

void Animation::update(float deltaTime) {
    if (frameCount <= 1) return;
    timer += deltaTime;
    if (timer >= frameDuration) {
        timer -= frameDuration;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void Animation::applyTo(sf::Sprite& sprite) {
    int col = currentFrame % columns;
    int row = currentFrame / columns;
    sprite.setTextureRect(sf::IntRect({ col * frameSize.x, row * frameSize.y }, frameSize));
}