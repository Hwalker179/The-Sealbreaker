#pragma once

#include <SDL3/SDL.h>

class Player
{
public:
    void update(float deltaTime, const SDL_FRect& room);
    void render(SDL_Renderer* renderer) const;

private:
    SDL_FRect bounds{ 304.0f, 224.0f, 32.0f, 32.0f };
    float speed = 180.0f;
};