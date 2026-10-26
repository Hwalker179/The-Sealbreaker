#pragma once

#include <SDL3/SDL.h>

class Player
{
public:
    void update(float deltaTime, const SDL_FRect& room);
    void render(SDL_Renderer* renderer) const;

    const SDL_FRect& getBounds() const
    {
        return bounds;
    }

    void setPosition(float x, float y)
    {
        bounds.x = x;
        bounds.y = y;
    }

private:
    SDL_FRect bounds{ 304.0f, 224.0f, 32.0f, 32.0f };
    float speed = 180.0f;
};