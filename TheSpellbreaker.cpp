#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Player.h"

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("The Sealbreaker", 640, 480, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Room boundary: x, y, width, height
    const SDL_FRect room{ 40.0f, 40.0f, 560.0f, 400.0f };

    Player player;

    Uint64 previousTime = SDL_GetTicks();

    bool running = true;

    while (running)
    {
        // Handle input and window events
        SDL_Event event{};

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN
                && event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }
        }

        if (!running)
        {
            break;
        }

        // Calculate elapsed time in seconds
        const Uint64 currentTime = SDL_GetTicks();
        float deltaTime = static_cast<float>(currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        // Limit movement jumps after a long pause or debugger stop
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        player.update(deltaTime, room);

        // Clear the screen to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Use white for the walls and player
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Draw the thin room outline
        SDL_RenderRect(renderer, &room);

        player.render(renderer);

        // Display the completed frame
        SDL_RenderPresent(renderer);

        // Simple delay to avoid running the loop unnecessarily fas
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}