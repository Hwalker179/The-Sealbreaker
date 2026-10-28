#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <string>

class Player
{
public:

    bool loadSprite(SDL_Renderer* renderer, const std::string& path);
    void unloadSprite();

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

    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>
        sprite{ nullptr, SDL_DestroyTexture };

    SDL_FRect bounds{ 304.0f, 224.0f, 32.0f, 32.0f };
    float speed = 180.0f;
};