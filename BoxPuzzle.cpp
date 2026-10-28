#include "BoxPuzzle.h"

void BoxPuzzle::render(SDL_Renderer* renderer) const
{
    constexpr float tileSize = 32.0f;
    constexpr float startX = 144.0f;
    constexpr float startY = 96.0f;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int row = 0; row < 9; ++row)
    {
        for (int column = 0; column < 11; ++column)
        {
            // Two horizontal tracks and two vertical tracks
            const bool isTrack =
                row == 2 || row == 6 ||
                column == 3 || column == 7;

            if (isTrack)
            {
                const SDL_FRect tile{
                    startX + column * tileSize,
                    startY + row * tileSize,
                    tileSize,
                    tileSize
                };

                SDL_RenderRect(renderer, &tile);
            }
        }
    }
}