#include "Player.h"
#include <algorithm>
#include <cmath>

void Player::update(float deltaTime, const SDL_FRect& room)
{
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float dx = 0.0f;
    float dy = 0.0f;

    if (keys[SDL_SCANCODE_W]) dy -= 1.0f;
    if (keys[SDL_SCANCODE_S]) dy += 1.0f;
    if (keys[SDL_SCANCODE_A]) dx -= 1.0f;
    if (keys[SDL_SCANCODE_D]) dx += 1.0f;

    // Keep diagonal movement at the same speed as straight movement
    const float length = std::sqrt(dx * dx + dy * dy);

    if (length > 0.0f)
    {
        bounds.x += (dx / length) * speed * deltaTime;
        bounds.y += (dy / length) * speed * deltaTime;
    }

    // Keep the entire player inside the room's outline
    bounds.x = std::clamp(
        bounds.x, room.x + 1.0f,
        room.x + room.w - bounds.w - 1.0f);

    bounds.y = std::clamp(
        bounds.y, room.y + 1.0f,
        room.y + room.h - bounds.h - 1.0f);
}

void Player::render(SDL_Renderer* renderer) const
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &bounds);
}