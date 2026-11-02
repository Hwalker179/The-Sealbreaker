#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <array>

class RunePuzzle
{
public:
    void render(SDL_Renderer* renderer) const;
    bool loadClue(const std::string& path);
    void interact(const SDL_FRect& player, SDL_Window* window);
    bool isSolved() const { return solved; }

    const std::string& getClue() const
    {
        return clue;
    }

    bool isNearNote(const SDL_FRect& player) const
    {
        return isNear(player, note);
    }

private:
    SDL_FRect note{ 308.0f, 224.0f, 24.0f, 32.0f };
    std::string clue = "The note could not be loaded.";

    bool isNear(
        const SDL_FRect& player, const SDL_FRect& object) const;

    // Symbols: Mountain = 0, River = 1, Flame = 2, Moon = 3.
    const std::array<int, 4> solution{ 0, 1, 2, 3 };

    int progress = 0;
    bool solved = false;

    SDL_FRect runeBounds(int index) const;
};