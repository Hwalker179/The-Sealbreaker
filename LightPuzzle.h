#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <string>

class LightPuzzle
{
public:
    bool loadClue(const std::string& path);
    void interact(const SDL_FRect& player, SDL_Window* window);
    void render(SDL_Renderer* renderer) const;

    bool isSolved() const { return solved; }

private:
    SDL_FRect lightBounds(int index) const;
    bool isNear(const SDL_FRect& player, const SDL_FRect& object) const;

    std::array<bool, 4> lights{ false, false, false, false };

    // Puzzle Answer
    const std::array<bool, 4> solution{ true, false, true, false };

    SDL_FRect note{ 100.0f, 320.0f, 24.0f, 32.0f };
    std::string clue = "The note could not be loaded.";
    bool solved = false;
};