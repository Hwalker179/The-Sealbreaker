#include "LightPuzzle.h"
#include <cstdlib>
#include <fstream>
#include <iostream>

bool LightPuzzle::loadClue(const std::string& path)
{
    std::ifstream file(path);
    std::string text;

    if (!std::getline(file, text, '\0') || text.empty())
    {
        std::cerr << "Could not read clue: " << path << '\n';
        return false;
    }

    clue = text;
    return true;
}

SDL_FRect LightPuzzle::lightBounds(int index) const
{
    return {
        54.0f + static_cast<float>(index) * 100.0f,
        120.0f, 32.0f, 32.0f
    };
}

bool LightPuzzle::isNear(
    const SDL_FRect& player, const SDL_FRect& object) const
{
    const float dx =
        (player.x + player.w / 2.0f) - (object.x + object.w / 2.0f);

    const float dy =
        (player.y + player.h / 2.0f) - (object.y + object.h / 2.0f);

    return dx * dx + dy * dy <= 48.0f * 48.0f;
}

void LightPuzzle::interact(
    const SDL_FRect& player, SDL_Window* window)
{
    if (isNear(player, note))
    {
        if (!SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_INFORMATION,
            "Dungeon note", clue.c_str(), window))
        {
            std::cerr << "Could not display note: "
                << SDL_GetError() << '\n';
        }

        return;
    }

    if (solved)
    {
        return;
    }

    for (int i = 0; i < static_cast<int>(lights.size()); ++i)
    {
        if (isNear(player, lightBounds(i)))
        {
            lights[i] = !lights[i];
            solved = (lights == solution);
            return;
        }
    }
}

void LightPuzzle::render(SDL_Renderer* renderer) const
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < static_cast<int>(lights.size()); ++i)
    {
        const SDL_FRect light = lightBounds(i);
        const float cx = light.x + light.w / 2.0f;
        const float cy = light.y + light.h / 2.0f;

        // Crystal outline
        const SDL_FPoint crystal[] = {
            {cx,         cy - 16.0f},
            {cx + 16.0f, cy},
            {cx,         cy + 16.0f},
            {cx - 16.0f, cy},
            {cx,         cy - 16.0f}
        };

        SDL_RenderLines(renderer, crystal, 5);

        // Small stand beneath the crystal
        const SDL_FRect stand{
            cx - 11.0f, cy + 20.0f, 22.0f, 4.0f
        };

        SDL_RenderFillRect(renderer, &stand);

        if (lights[i])
        {
            // Fill a smaller diamond
            for (int row = -11; row <= 11; ++row)
            {
                const float halfWidth =
                    static_cast<float>(11 - std::abs(row));

                SDL_RenderLine(
                    renderer,
                    cx - halfWidth, cy + static_cast<float>(row),
                    cx + halfWidth, cy + static_cast<float>(row));
            }
        }
    }

    // Note
    SDL_RenderRect(renderer, &note);

    SDL_RenderLine(
        renderer, note.x + 5.0f, note.y + 10.0f,
        note.x + 19.0f, note.y + 10.0f);

    SDL_RenderLine(
        renderer, note.x + 5.0f, note.y + 18.0f,
        note.x + 19.0f, note.y + 18.0f);
}