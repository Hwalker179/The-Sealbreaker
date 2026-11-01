#pragma once

#include <SDL3/SDL.h>
#include <string>

class RunePuzzle
{
public:
    void render(SDL_Renderer* renderer) const;
    bool loadClue(const std::string& path);
    void interact(const SDL_FRect& player, SDL_Window* window);

private:
    SDL_FRect note{ 308.0f, 224.0f, 24.0f, 32.0f };
    std::string clue = "The note could not be loaded.";

    bool isNear(
        const SDL_FRect& player, const SDL_FRect& object) const;
};