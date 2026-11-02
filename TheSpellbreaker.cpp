#include <iostream>
#include <sstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Player.h"
#include <string>
#include <cstring>
#include "LightPuzzle.h"
#include "BoxPuzzle.h"
#include "RunePuzzle.h"

void drawCenteredText(
    SDL_Renderer* renderer, float y, const char* text)
{
    constexpr float windowWidth = 640.0f;
    constexpr float characterWidth = 8.0f;

    const float textWidth =
        static_cast<float>(std::strlen(text)) * characterWidth;

    const float x = (windowWidth - textWidth) / 2.0f;

    SDL_RenderDebugText(renderer, x, y, text);
}

void drawCoinPile(SDL_Renderer* renderer, float x, float y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Three stacks of small flat coins
    for (int stack = 0; stack < 3; ++stack)
    {
        const int coinCount = (stack == 1) ? 4 : 2;

        for (int coin = 0; coin < coinCount; ++coin)
        {
            const SDL_FRect bounds{
                x + stack * 12.0f,
                y - coin * 5.0f,
                10.0f,
                4.0f
            };

            SDL_RenderRect(renderer, &bounds);
        }
    }
}

void drawTreasureChest(SDL_Renderer* renderer, float x, float y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Chest body
    const SDL_FRect body{ x, y + 20.0f, 64.0f, 32.0f };
    SDL_RenderRect(renderer, &body);

    // Raised lid
    const SDL_FPoint lid[] = {
        {x,         y + 14.0f},
        {x + 6.0f,  y},
        {x + 58.0f, y},
        {x + 64.0f, y + 14.0f},
        {x,         y + 14.0f}
    };

    SDL_RenderLines(renderer, lid, 5);

    // Metal bands on the body
    SDL_RenderLine(
        renderer, x + 12.0f, y + 20.0f,
        x + 12.0f, y + 52.0f);

    SDL_RenderLine(
        renderer, x + 52.0f, y + 20.0f,
        x + 52.0f, y + 52.0f);

    // Clasp
    const SDL_FRect clasp{ x + 28.0f, y + 20.0f, 8.0f, 10.0f };
    SDL_RenderFillRect(renderer, &clasp);

    // Treasure visible inside the open chest
    for (int i = 0; i < 5; ++i)
    {
        const SDL_FRect coin{
            x + 5.0f + i * 11.0f,
            y + 16.0f,
            9.0f,
            3.0f
        };

        SDL_RenderFillRect(renderer, &coin);
    }
}

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

    enum class Screen
    {
        Menu,
        Instructions,
        Note,
        Playing
    };

    Screen screen = Screen::Menu;
    int selectedOption = 0;
    std::string activeNote;

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

    BoxPuzzle secondPuzzle;
    secondPuzzle.loadClue("assets/room2_note.txt");

    RunePuzzle thirdPuzzle;
    thirdPuzzle.loadClue("assets/room3_note.txt");

    Uint64 previousTime = SDL_GetTicks();

    // Victory pedestal
    const SDL_FRect pedestal{ 288.0f, 208.0f, 64.0f, 64.0f };

    bool gameCompleted = false;

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

            else if (screen != Screen::Playing)
            {
                if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
                {
                    if (screen == Screen::Menu)
                    {
                        switch (event.key.key)
                        {
                        case SDLK_UP:
                            selectedOption = (selectedOption + 2) % 3;
                            break;

                        case SDLK_DOWN:
                            selectedOption = (selectedOption + 1) % 3;
                            break;

                        case SDLK_RETURN:
                            if (selectedOption == 0)
                            {
                                screen = Screen::Playing;
                                previousTime = SDL_GetTicks();

                                SDL_SetWindowTitle(
                                    window, "The Sealbreaker - Room 1/4");
                            }
                            else if (selectedOption == 1)
                            {
                                screen = Screen::Instructions;
                            }
                            else
                            {
                                running = false;
                            }
                            break;

                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        }
                    }
                    else if (screen == Screen::Instructions ||
                        screen == Screen::Note)
                    {
                        if (event.key.key == SDLK_ESCAPE ||
                            event.key.key == SDLK_RETURN)
                        {
                            screen = (screen == Screen::Note)
                                ? Screen::Playing
                                : Screen::Menu;

                            previousTime = SDL_GetTicks();
                        }
                    }
                }
            }

            else if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }

            else if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_E &&
                !event.key.repeat)
            {
                const SDL_FRect position = player.getBounds();

                if (currentRoom == 0)
                {
                    if (firstPuzzle.isNearNote(position))
                    {
                        activeNote = firstPuzzle.getClue();
                        screen = Screen::Note;
                    }
                    else
                    {
                        firstPuzzle.interact(position, window);
                    }
                }
                else if (currentRoom == 1)
                {
                    if (secondPuzzle.isNearNote(position))
                    {
                        activeNote = secondPuzzle.getClue();
                        screen = Screen::Note;
                    }
                }
                else if (currentRoom == 2)
                {
                    if (thirdPuzzle.isNearNote(position))
                    {
                        activeNote = thirdPuzzle.getClue();
                        screen = Screen::Note;
                    }
                    else
                    {
                        thirdPuzzle.interact(position, window);
                    }
                }

                previousTime = SDL_GetTicks();
            }
            
            else if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_R &&
                !event.key.repeat &&
                currentRoom == 1 &&
                !secondPuzzle.isSolved())
            {
                secondPuzzle.reset();

                // Return to a clear position outside the tracks
                player.setPosition(80.0f, 224.0f);
            }
        }

        if (!running)
        {
            break;
        }

        if (screen != Screen::Playing)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            if (screen == Screen::Menu)
            {
                drawCenteredText(renderer, 130.0f, "THE SEALBREAKER");

                const char* options[] = {
                    "Start Game",
                    "Instructions",
                    "Exit"
                };

                for (int i = 0; i < 3; ++i)
                {
                    const float y = 210.0f + i * 40.0f;

                    drawCenteredText(renderer, y, options[i]);

                    if (i == selectedOption)
                    {
                        const float textWidth =
                            static_cast<float>(std::strlen(options[i])) * 8.0f;

                        const float textLeft = (640.0f - textWidth) / 2.0f;

                        SDL_RenderDebugText(
                            renderer, textLeft - 24.0f, y, ">");
                    }
                }

                drawCenteredText(
                    renderer, 380.0f,
                    "Up/Down to select - Enter to confirm");
            }
            else if (screen == Screen::Instructions)
            {
                drawCenteredText(renderer, 100.0f, "INSTRUCTIONS");

                const char* instructions[] = {
                    "WASD - Move",
                    "E - Read notes and activate nearby symbols",
                    "Walk into boxes to push them",
                    "R - Reset the unfinished box puzzle",
                    "Solve all three puzzles to reach the treasure",
                    "Stand fully on the final pedestal to finish"
                };

                for (int i = 0; i < 6; ++i)
                {
                    drawCenteredText(
                        renderer,
                        160.0f + i * 32.0f,
                        instructions[i]);
                }

                drawCenteredText(
                    renderer, 400.0f,
                    "Press Enter or Escape to return");
            }

            else if (screen == Screen::Note)
            {
                drawCenteredText(renderer, 80.0f, "DUNGEON NOTE");

                std::istringstream lines(activeNote);
                std::string line;
                float y = 140.0f;

                while (std::getline(lines, line))
                {
                    if (!line.empty() && line.back() == '\r')
                    {
                        line.pop_back();
                    }

                    drawCenteredText(renderer, y, line.c_str());
                    y += 24.0f;
                }

                drawCenteredText(
                    renderer, 420.0f,
                    "Press Enter or Escape to return");
            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16);

            // Skip gameplay while a menu screen is displayed
            previousTime = SDL_GetTicks();
            continue;
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

        const SDL_FRect oldPosition = player.getBounds();

        player.update(deltaTime, room);

        if (currentRoom == 1)
        {
            const SDL_FRect newPosition = player.getBounds();

            // Horizontal movement
            player.setPosition(newPosition.x, oldPosition.y);

            const int pushX =
                (newPosition.x > oldPosition.x) ? 1 :
                (newPosition.x < oldPosition.x) ? -1 : 0;

            if (pushX != 0)
            {
                secondPuzzle.tryPush(player.getBounds(), pushX, 0);
            }

            if (secondPuzzle.collidesWithBox(player.getBounds()))
            {
                player.setPosition(oldPosition.x, oldPosition.y);
            }

            // Vertical movement
            const float acceptedX = player.getBounds().x;
            player.setPosition(acceptedX, newPosition.y);

            const int pushY =
                (newPosition.y > oldPosition.y) ? 1 :
                (newPosition.y < oldPosition.y) ? -1 : 0;

            if (pushY != 0)
            {
                secondPuzzle.tryPush(player.getBounds(), 0, pushY);
            }

            if (secondPuzzle.collidesWithBox(player.getBounds()))
            {
                player.setPosition(acceptedX, oldPosition.y);
            }
        }

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
                (currentRoom != 1 || secondPuzzle.isSolved()) &&
                (currentRoom != 2 || thirdPuzzle.isSolved()) &&
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

        if (currentRoom == 1)
        {
            secondPuzzle.render(renderer);

            if (!secondPuzzle.isSolved())
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                SDL_RenderLine(
                    renderer, right, openingTop, right, openingBottom);
            }
        }

        if (currentRoom == 2)
        {
            thirdPuzzle.render(renderer);

            if (!thirdPuzzle.isSolved())
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

                SDL_RenderLine(
                    renderer, right, openingTop, right, openingBottom);
            }
        }

        if (currentRoom == 3)
        {

            drawTreasureChest(renderer, 288.0f, 130.0f);

            drawCoinPile(renderer, 150.0f, 150.0f);
            drawCoinPile(renderer, 450.0f, 150.0f);
            drawCoinPile(renderer, 125.0f, 350.0f);
            drawCoinPile(renderer, 450.0f, 350.0f);
            drawCoinPile(renderer, 400.0f, 300.0f);
            drawCoinPile(renderer, 500.0f, 400.0f);
            drawCoinPile(renderer, 100.0f, 200.0f);
            drawCoinPile(renderer, 200.0f, 200.0f);
            drawCoinPile(renderer, 300.0f, 100.0f);
            drawCoinPile(renderer, 400.0f, 100.0f);
            drawCoinPile(renderer, 400.0f, 200.0f);
            drawCoinPile(renderer, 150.0f, 400.0f);
            drawCoinPile(renderer, 300.0f, 400.0f);
            drawCoinPile(renderer, 250.0f, 380.0f);
            drawCoinPile(renderer, 350.0f, 350.0f);
            drawCoinPile(renderer, 475.0f, 250.0f);
            drawCoinPile(renderer, 525.0f, 200.0f);
            drawCoinPile(renderer, 510.0f, 300.0f);
            drawCoinPile(renderer, 50.0f, 75.0f);
            drawCoinPile(renderer, 175.0f, 90.0f);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            // Outer edge of the pedestal
            SDL_RenderRect(renderer, &pedestal);

            // Inner edge 
            const SDL_FRect inner{
                pedestal.x + 6.0f,
                pedestal.y + 6.0f,
                pedestal.w - 12.0f,
                pedestal.h - 12.0f
            };

            SDL_RenderRect(renderer, &inner);
        }

        player.render(renderer);

        // Display the completed frame
        SDL_RenderPresent(renderer);

        if (currentRoom == 3 && !gameCompleted)
        {
            const SDL_FRect playerBounds = player.getBounds();

            const bool onPedestal =
                playerBounds.x >= pedestal.x &&
                playerBounds.y >= pedestal.y &&
                playerBounds.x + playerBounds.w <= pedestal.x + pedestal.w &&
                playerBounds.y + playerBounds.h <= pedestal.y + pedestal.h;

            if (onPedestal)
            {
                gameCompleted = true;

                if (!SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_INFORMATION,
                    "The final seal is broken.",
                    "Congratulations! You have completed The Sealbreaker.\n\n"
                    "Take your treasure and go home!",
                    window))
                {
                    std::cerr << "Could not display completion message: "
                        << SDL_GetError() << '\n';
                }

                running = false;
            }
        }

        // Simple delay to avoid running the loop unnecessarily fas
        SDL_Delay(16);
    }

    player.unloadSprite();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}