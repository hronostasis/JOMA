#include "Animation.h"

Animation::Animation(sf::Texture& texture, int columns, int rows, float frameDuration, bool loop)
    : columns(columns), frameDuration(frameDuration), loop(loop)
{
    sf::Vector2u texSize = texture.getSize();
    frameSize = { (int)(texSize.x / columns), (int)(texSize.y / rows) };
    frameCount = columns * rows;
}

void Animation::update(float deltaTime) {
    if (frameCount <= 1 || finished) return;
    timer += deltaTime;
    if (timer >= frameDuration) {
        timer -= frameDuration;
        currentFrame++;
        if (currentFrame >= frameCount) {
            if (loop) currentFrame = 0;
            else { currentFrame = frameCount - 1; finished = true; }
        }
    }
}

void Animation::applyTo(sf::Sprite& sprite) {
    int col = currentFrame % columns;
    int row = currentFrame / columns;
    sprite.setTextureRect(sf::IntRect({ col * frameSize.x, row * frameSize.y }, frameSize));
}

void Animation::reset() {
    currentFrame = 0;
    timer = 0.f;
    finished = false;
}