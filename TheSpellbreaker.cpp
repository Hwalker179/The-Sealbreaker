#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Player.h"
#include <string>
#include "LightPuzzle.h"

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("The Sealbreaker - Room 1/3", 640, 480, 0);
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

    constexpr int roomCount = 4;
    int currentRoom = 0;

    // Openings in the middle of the side walls
    constexpr float openingTop = 200.0f;
    constexpr float openingBottom = 280.0f;

    Player player;
    player.loadSprite(renderer, "assets/player.png");

    LightPuzzle firstPuzzle;
    firstPuzzle.loadClue("assets/room1_note.txt");

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

            else if (event.type == SDL_EVENT_KEY_DOWN
                && event.key.scancode == SDL_SCANCODE_E
                && !event.key.repeat
                && currentRoom == 0)
            {
                firstPuzzle.interact(player.getBounds(), window);

                // Avoid including time spent reading the note in movement.
                previousTime = SDL_GetTicks();
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

        const SDL_FRect position = player.getBounds();
        const int previousRoom = currentRoom;

        // The whole player must fit inside the opening
        const bool insideOpening =
            position.y >= openingTop &&
            position.y + position.h <= openingBottom;

        if (insideOpening)
        {
            // Walk right into next
            if (currentRoom < roomCount - 1 &&
                (currentRoom != 0 || firstPuzzle.isSolved()) &&
                position.x + position.w >= room.x + room.w - 1.0f)
            {
                ++currentRoom;
                player.setPosition(room.x + 2.0f, position.y);
            }
            // Walk left into previous
            else if (currentRoom > 0 &&
                position.x <= room.x + 1.0f)
            {
                --currentRoom;
                player.setPosition(
                    room.x + room.w - position.w - 2.0f,
                    position.y);
            }
        }

        // Changes room title
        if (currentRoom != previousRoom)
        {
            const std::string title =
                "The Sealbreaker - Room " +
                std::to_string(currentRoom + 1) + "/4";

            SDL_SetWindowTitle(window, title.c_str());
        }

        // Clear the screen to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Use white for the walls and player
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        const float left = room.x;
        const float right = room.x + room.w;
        const float top = room.y;
        const float bottom = room.y + room.h;

        // Top and bottom walls
        SDL_RenderLine(renderer, left, top, right, top);
        SDL_RenderLine(renderer, left, bottom, right, bottom);

        // Left wall: opening only if there is a previous room
        if (currentRoom > 0)
        {
            SDL_RenderLine(renderer, left, top, left, openingTop);
            SDL_RenderLine(renderer, left, openingBottom, left, bottom);
        }
        else
        {
            SDL_RenderLine(renderer, left, top, left, bottom);
        }

        // Right wall: opening only if there is a next room
        if (currentRoom < roomCount - 1)
        {
            SDL_RenderLine(renderer, right, top, right, openingTop);
            SDL_RenderLine(renderer, right, openingBottom, right, bottom);
        }
        else
        {
            SDL_RenderLine(renderer, right, top, right, bottom);
        }

        if (currentRoom == 0)
        {
            firstPuzzle.render(renderer);

            // A white barrier seals the first room's right opening
            if (!firstPuzzle.isSolved())
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                SDL_RenderLine(
                    renderer, right, openingTop, right, openingBottom);
            }
        }

        player.render(renderer);

        // Display the completed frame
        SDL_RenderPresent(renderer);

        // Simple delay to avoid running the loop unnecessarily fas
        SDL_Delay(16);
    }

    player.unloadSprite();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}