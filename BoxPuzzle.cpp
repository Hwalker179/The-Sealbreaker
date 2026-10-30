#include "BoxPuzzle.h"

SDL_FRect BoxPuzzle::cellBounds(int column, int row) const
{
    constexpr float tileSize = 32.0f;
    constexpr float startX = 144.0f;
    constexpr float startY = 96.0f;

    return {
        startX + column * tileSize,
        startY + row * tileSize,
        tileSize,
        tileSize
    };
}

void BoxPuzzle::drawSymbol(
    SDL_Renderer* renderer,
    const SDL_FRect& tile,
    int symbol) const
{
    const float cx = tile.x + tile.w / 2.0f;
    const float cy = tile.y + tile.h / 2.0f;
    constexpr float size = 7.0f;

    switch (symbol)
    {
    case 0: // Plus
        SDL_RenderLine(renderer, cx - size, cy, cx + size, cy);
        SDL_RenderLine(renderer, cx, cy - size, cx, cy + size);
        break;

    case 1: // Cross
        SDL_RenderLine(
            renderer, cx - size, cy - size, cx + size, cy + size);
        SDL_RenderLine(
            renderer, cx - size, cy + size, cx + size, cy - size);
        break;

    case 2: // Diamond
        SDL_RenderLine(renderer, cx, cy - size, cx + size, cy);
        SDL_RenderLine(renderer, cx + size, cy, cx, cy + size);
        SDL_RenderLine(renderer, cx, cy + size, cx - size, cy);
        SDL_RenderLine(renderer, cx - size, cy, cx, cy - size);
        break;

    case 3: // Square
    {
        const SDL_FRect square{
            cx - size, cy - size, size * 2.0f, size * 2.0f
        };

        SDL_RenderRect(renderer, &square);
        break;
    }
    }
}

void BoxPuzzle::render(SDL_Renderer* renderer) const
{
    // Draw the hashtag floor
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int row = 0; row < 9; ++row)
    {
        for (int column = 0; column < 11; ++column)
        {
            const bool isTrack =
                row == 2 || row == 6 ||
                column == 3 || column == 7;

            if (isTrack)
            {
                const SDL_FRect tile = cellBounds(column, row);
                SDL_RenderRect(renderer, &tile);
            }
        }
    }

    // Draw matching floor slots
    for (int i = 0; i < 4; ++i)
    {
        const SDL_FRect tile =
            cellBounds(targets[i].column, targets[i].row);

        const SDL_FRect slot{
            tile.x + 3.0f, tile.y + 3.0f,
            tile.w - 6.0f, tile.h - 6.0f
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &slot);
        drawSymbol(renderer, tile, i);
    }

    // Draw white boxes with black symbols
    for (int i = 0; i < 4; ++i)
    {
        const SDL_FRect tile =
            cellBounds(boxes[i].column, boxes[i].row);

        const SDL_FRect box{
            tile.x + 2.0f, tile.y + 2.0f,
            tile.w - 4.0f, tile.h - 4.0f
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &box);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        drawSymbol(renderer, tile, i);
    }
    // Restore white for any drawing that follows
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

bool BoxPuzzle::collidesWithBox(const SDL_FRect& player) const
{
    for (const Cell& box : boxes)
    {
        const SDL_FRect tile = cellBounds(box.column, box.row);

        // Match the filled box drawn inside its floor tile
        const SDL_FRect bounds{
            tile.x + 2.0f, tile.y + 2.0f,
            tile.w - 4.0f, tile.h - 4.0f
        };

        // Checks if two rectangles overlap
        if (SDL_HasRectIntersectionFloat(&player, &bounds))
        {
            return true;
        }
    }

    return false;
}

void BoxPuzzle::tryPush(const SDL_FRect& player, int dx, int dy)
{
    if (solved)
    {
        return;
    }

    // Only allow one horizontal or vertical step
    if (!((dx == -1 || dx == 1) && dy == 0) &&
        !((dy == -1 || dy == 1) && dx == 0))
    {
        return;
    }

    for (Cell& box : boxes)
    {
        const SDL_FRect tile = cellBounds(box.column, box.row);

        const SDL_FRect bounds{
            tile.x + 2.0f, tile.y + 2.0f,
            tile.w - 4.0f, tile.h - 4.0f
        };

        if (!SDL_HasRectIntersectionFloat(&player, &bounds))
        {
            continue;
        }

        const int nextColumn = box.column + dx;
        const int nextRow = box.row + dy;

        // Stay inside the grid
        if (nextColumn < 0 || nextColumn >= 11 ||
            nextRow < 0 || nextRow >= 9)
        {
            return;
        }

        // Boxes can only move along the hashtag tracks
        const bool isTrack =
            nextRow == 2 || nextRow == 6 ||
            nextColumn == 3 || nextColumn == 7;

        if (!isTrack)
        {
            return;
        }

        // Do not push into another box
        for (const Cell& other : boxes)
        {
            if (other.column == nextColumn &&
                other.row == nextRow)
            {
                return;
            }
        }

        box.column = nextColumn;
        box.row = nextRow;

        // Each box must occupy its own matching target
        solved = true;

        for (int i = 0; i < static_cast<int>(boxes.size()); ++i)
        {
            if (boxes[i].column != targets[i].column ||
                boxes[i].row != targets[i].row)
            {
                solved = false;
                break;
            }
        }

        return;
    }
}

void BoxPuzzle::reset()
{
    if (solved)
    {
        return;
    }

    boxes = { {
        {3, 0},
        {7, 0},
        {0, 2},
        {0, 6}
    } };
}