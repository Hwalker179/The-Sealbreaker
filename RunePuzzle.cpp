#include "RunePuzzle.h"
#include <fstream>
#include <iostream>

bool RunePuzzle::loadClue(const std::string& path)
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

bool RunePuzzle::isNear(
    const SDL_FRect& player, const SDL_FRect& object) const
{
    const float dx =
        (player.x + player.w / 2.0f) - (object.x + object.w / 2.0f);

    const float dy =
        (player.y + player.h / 2.0f) - (object.y + object.h / 2.0f);

    return dx * dx + dy * dy <= 48.0f * 48.0f;
}

void RunePuzzle::interact(
    const SDL_FRect& player, SDL_Window* window)
{
    // New note handling
    if (isNear(player, note))
    {
        return;
    }

    if (solved)
    {
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        if (isNear(player, runeBounds(i)))
        {
            if (i == solution[progress])
            {
                ++progress;

                if (progress == static_cast<int>(solution.size()))
                {
                    solved = true;
                }
            }
            else
            {
                // A wrong choice clears the entire sequence
                progress = 0;
            }

            return;
        }
    }
}

void RunePuzzle::render(SDL_Renderer* renderer) const
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Mountain
    const SDL_FPoint mountain[] = {
        {140.0f, 180.0f},
        {170.0f, 130.0f},
        {200.0f, 180.0f},
        {140.0f, 180.0f}
    };

    SDL_RenderLines(renderer, mountain, 4);

    // Snow cap
    const SDL_FPoint snow[] = {
        {158.0f, 150.0f},
        {166.0f, 154.0f},
        {174.0f, 148.0f},
        {182.0f, 150.0f}
    };

    SDL_RenderLines(renderer, snow, 4);

    // River
    const SDL_FPoint riverLeft[] = {
        {448.0f, 130.0f},
        {438.0f, 142.0f},
        {450.0f, 155.0f},
        {440.0f, 168.0f},
        {448.0f, 180.0f}
    };

    const SDL_FPoint riverRight[] = {
        {472.0f, 130.0f},
        {462.0f, 142.0f},
        {474.0f, 155.0f},
        {464.0f, 168.0f},
        {472.0f, 180.0f}
    };

    SDL_RenderLines(renderer, riverLeft, 5);
    SDL_RenderLines(renderer, riverRight, 5);

    // Flame outer shape
    const SDL_FPoint flame[] = {
        {170.0f, 290.0f},
        {180.0f, 310.0f},
        {190.0f, 304.0f},
        {194.0f, 326.0f},
        {186.0f, 341.0f},
        {170.0f, 348.0f},
        {154.0f, 341.0f},
        {146.0f, 326.0f},
        {157.0f, 307.0f},
        {160.0f, 320.0f},
        {170.0f, 290.0f}
    };

    SDL_RenderLines(renderer, flame, 11);

    // Flame inner shape
    const SDL_FPoint innerFlame[] = {
        {170.0f, 320.0f},
        {179.0f, 335.0f},
        {170.0f, 342.0f},
        {161.0f, 335.0f},
        {170.0f, 320.0f}
    };

    SDL_RenderLines(renderer, innerFlame, 5);

    // Moon
    const SDL_FPoint moon[] = {
        {476.0f, 292.0f},
        {458.0f, 290.0f},
        {442.0f, 300.0f},
        {434.0f, 318.0f},
        {439.0f, 337.0f},
        {455.0f, 348.0f},
        {476.0f, 344.0f},
        {462.0f, 337.0f},
        {454.0f, 324.0f},
        {454.0f, 310.0f},
        {462.0f, 298.0f},
        {476.0f, 292.0f}
    };

    SDL_RenderLines(renderer, moon, 12);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderRect(renderer, &note);

    SDL_RenderLine(
        renderer, note.x + 5.0f, note.y + 10.0f,
        note.x + 19.0f, note.y + 10.0f);

    SDL_RenderLine(
        renderer, note.x + 5.0f, note.y + 18.0f,
        note.x + 19.0f, note.y + 18.0f);

    // Four indicators above the centre of the room
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < 4; ++i)
    {
        const SDL_FRect indicator{
            276.0f + i * 24.0f,
            70.0f,
            16.0f,
            16.0f
        };

        if (i < progress)
        {
            SDL_RenderFillRect(renderer, &indicator);
        }
        else
        {
            SDL_RenderRect(renderer, &indicator);
        }
    }

}

// Interaction areas
SDL_FRect RunePuzzle::runeBounds(int index) const
{
    switch (index)
    {
    case 0: // Mountain
        return { 140.0f, 130.0f, 60.0f, 50.0f };

    case 1: // River
        return { 434.0f, 130.0f, 44.0f, 50.0f };

    case 2: // Flame
        return { 146.0f, 290.0f, 48.0f, 58.0f };

    case 3: // Moon
        return { 434.0f, 290.0f, 44.0f, 58.0f };

    default:
        return {};
    }
}