#pragma once

#include <SDL3/SDL.h>
#include <array>
#include <string>

class BoxPuzzle
{
public:
    void render(SDL_Renderer* renderer) const;
    bool collidesWithBox(const SDL_FRect& player) const;
    void tryPush(const SDL_FRect& player, int dx, int dy);
    void reset();
    bool loadClue(const std::string& path);
    void interact(const SDL_FRect& player, SDL_Window* window);
    bool isSolved() const { return solved; }
private:
    struct Cell
    {
        int column;
        int row;
    };

    std::array<Cell, 4> boxes{ {
        {3, 0},  // Top left - plus
        {7, 0},  // Top right - cross
        {0, 2},  // Left upper - diamond
        {0, 6}   // Left lower - square
    } };

    const std::array<Cell, 4> targets{{
    {10, 6}, // Plus: lower right slot
    {3, 8}, // Cross: upper right slot
    {7, 8},  // Diamond: bottom right slot
    {10, 2}   // Square: bottom left slot
    } };

    SDL_FRect cellBounds(int column, int row) const;

    void drawSymbol(
        SDL_Renderer* renderer,
        const SDL_FRect& tile,
        int symbol) const;

    SDL_FRect note{ 308.0f, 224.0f, 24.0f, 32.0f };
    std::string clue = "The note could not be loaded.";

    bool isNear(
        const SDL_FRect& player, const SDL_FRect& object) const;

    bool solved = false;
};
