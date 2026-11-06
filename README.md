# The Sealbreaker

A top down 2D puzzle adventure game.
The player explores three puzzle rooms breaking a magical seal each time to reach the next room.
The puzzles involve a True or false game with lights, pushing boxes to slots, and a combination lock.
Once they beat all three puzzles they gain access to the treasure room and can complete the game.

# Running the game

1. Extract the zip
2. Open the game folder
3. Run RunGame.bat or launch TheSpellbreaker.exe from that folder

# Building from Source

1. Open TheSpellbreaker.sln in Visual Studio
2. Ensure the Desktop development with C++ workload is installed
3. Select Release and x64
4. Use C++17 or later
5. Build the solution

# Youtube video link: https://www.youtube.com/watch?v=g3dzeuDdIwU
Sorry for the not great quality, my pc is not the best, working on that.

# Github link: https://github.com/Hwalker179/The-Sealbreaker

# Trello link: https://trello.com/b/s7ZhV4Q9/hugo-walker-comp3016-roadmap 

# Week 2 proposal:

CW1 Idea Proposal
Game Title – The Sealbreaker

Genre – 2D top down fantasy puzzle game

Synopsis – The player is a mage exploring a dungeon in search of treasure. The dungeon
has 3 connected rooms each with a magically sealed door that can only be opened by
breaking the spell / solving its puzzle. Opening the final door reveals the treasure and
completes the game.

Core Gameplay Loop – Explore the room, examine clues, interact with magic objects,
solve the puzzle, break the seal on the door, then progress to the next room.
Intended Mechanics – Player movement WASD, Object interactions, Three room
puzzles, Sealed doors, Clues or hints / Lore, Resetting the room

Intended technical features – C++ and SDL3 implementation, OOP using encapsulation,
inheritance and polymorphism, collision detection, menu, pause, gameplay and victory
states, input handling and validation with error messages for missing or invalid data,
documented build instructions, testing and known issues

Architecture overview
Game – manages the main loop, input, rendering and progression between rooms
Player – stores player position and handles movements and interactions
Room – loads its layout and puzzle, manages collisions and contains its puzzle and door
Puzzle – Defines the shares interface for interacting, resetting and checking puzzles
Door – tracks if its seal is locked or broken

External libraries – SDL3, maybe SDL3_image and SDL3_ttf

Intended use of AI – initial brainstorming, explanations of unfamiliar concepts and
suggestions for debugging and testing. Any ai-assisted code will be reviewed,
understood and tested before use and documented.

# Changes from the original proposal

My proposal didn't specify what the puzzles were going to be individually. These are:
- A six light puzzle where the player has to read a clue and to figure out which lights should be on
or off depending on if the statement is true or false.
- A box pushing puzzle where the player must move them to the corresponding symbol slot by sliding them
across hashtag shaped tracks.
- A rune puzzle where the player has to activate runes on the floor in a specific order which is in the clue.

Architechture changes:
I originally proposed that I would use seperate room, puzzle and door classes, including inheritance 
and polymorphism. Instead I used seperate LightPuzzle, BoxPuzzle and RunePuzzle classes, with the main loop, screen management and room transitions being handled in my main file TheSpellbreaker.cpp. 
This let me keep implementation straightforward while also letting each puzzle manage its own state and behaviour.
Doors are made by a barrier and room transition conditions that query each puzzles isSolved method, instead of
being seperate door objects. The rooms share one boundary layout and are differentiated with a current room number
instead of room objects, however if I was to expand the game, I would implement a room class to allow easier creation of a larger amount of levels and to seperate behaviour more clearly.
I made the reset conditional not just avaliable in any puzzle at any point so it is only avaliable to the player on demand in the box sliding segment and automatically activates when you input the incorrect sequence in the rune
puzzle.

Development roadmap -
https://trello.com/invite/b/6ada706270401348e25e0fcb/ATTIaecbbc45dc7a110c00049945312802c19CA105B4/hugo-walker-comp3016-roadmap

# Dependencies used:
SDL3, SDL3-devel-3.4.16-VC.zip, Window, input, rendering and timing
SDL3_image, SDL3_image-devel-3.4.8-VC.zip, Loading a player sprite

# Assets
Player sprite - player.png - made by Hugo Walker
Note text - made by Hugo Walker
Code Rune symbols and treasure chest - chatgpt generated shapes

# Game programming patterns:

My game uses a continuous game loop in TheSpellbreaker.cpp controlld by the running variable. Each iteration
processes SDL3 events, updates gameplay and renders the current screen. This allows movement and puzzle interactions
to respond while the game contiunes drawing frames.
The loop calculates time using SDL_GetTicks and passes deltaTime to the players update method. Movement is
calculated using speed multiplied by elapsed time, rather than a fixed distance per frame. I also limit deltaTime
to reduce large jumps after an interruption. The short delay at the end reduces unnecessary looping.

I used a simple finite state machine for screen state management which is represented by the screen enum.
The current screen determines which input and rendering code runs. For example, the enter key can start the
game from the menu screen, but it can also close a note during gameplay. When the menu, instructions or a note
are on screen, the loop skips gameplay updates to prevent a player from moving while reading.
For a larger project, I would use seperate screen classes to reduce the amount of conditional code in main.

The player class seperates movement behaviour into update() and drawing into render() where the main loop calls
these at the appropriate stages of each frame which keeps movement calculations seperate from visual presentation.
For example when I added the players sprite and added flipping it didnt requite changing the basic movement
calculations and when I launched the game without the assets the white box that was originally replaced still functioned.

Each puzzle stores its own data, like the states of the lights in the first puzzle or box positions from the second.
The main file calls methods like interact(), render() and isSolved() instead of directly changing private variables.
isSolved() provides a consistent way for room transition code to check puzzle state. 

This structure made it possible to develop the puzzles seperately, although the main file still handles alot of
responsibilities. A future improvement would be to move the main loop and room management into their own classes.

# Game mechanics and how they are coded:

Player movement and room boundaries
The player moves using WASD. In Player::update() the keyboard inputs determine horizontal and vertical movement
and while moving diagonally, the direction is normalised so that the player doesn't move faster than normal.
The players position is updated using movement speed multiplied by deltaTime. After the player moves,
clamp() restricts the players rectangle hitbox to the rooms boundaries. This ensures the entire player stays inside.
It also remembers its last horizontal direction, the sprite is the flipped depending on if you were walking left
or right.

Room transitions and sealed doors
I used a currentRoom index to select which puzzle is active. To move between rooms, the player has to walk into
one of the open side boundaries with the entire character in the doorway. When you move through a door, the
currentRoom is changed and it places the player in the opposite side of the next room. This also lets the player
move back to already solved rooms. Each puzzles sealed door is restricted by its own isSolved():
(currentRoom != 1 || secondPuzzle.isSolved()), this condition means the second puzzle only restricts forward
movement from room 2. A white line is drawn over the exit while its sealed to show obviously you can't pass yet,
while the transition check actually prevents progression.

Light puzzle
This puzzle stores six light states in a boolean array, where true = on and false = off, A second array stores
the answer needed. When you press E near one of the crystals, its state is toggled: lights[i] = !lights[i];
the current array is then compared against the solution: solved = (lights == solution); and if all 6 lights
match then the exit unlocks and the states of the lights are locked so it stays solved if the player comes back.

Box puzzle
This puzzle stores each box and target space using grid coordinates, with hashtag shaped tracks on the floor.
Player movement is checked seperately on the horizontal and vertical axis. If the players posititon overlaps a box,
tryPush() will try to move that box one space in the same direction you're moving. If the space the box is pushed
to is not part of the track or another box is already there then the push will not go through.
After a push each boxes coordinates are compared with their target, the symbol on the box and the target have
to match to visually represent this. Pressing R in this room at anypoint before the puzzle is completed
successfully will reset the boxes to their original positions using BoxPuzzle::reset().

Rune puzzle
The final puzzle gives each rune its own identifier and stores the needed input order in a array, while a
progress variable stores how many correct selections the player has made. When you press E near a rune, its
identifier is compared with the next expected rune: if (i == solution[progress]), if its correct then progress
increases and one of the indicators is filled, if not then progress is reset to 0. Further input is ignored
once the puzzle is complete.

Notes and interaction
Each puzzle loads its clue from a text file using ifstream. Keeping clues outside of the source code let me
change them as I went to better fit the puzzles or if I made changes to them like making it 6 lights instead of 4.
Interaction distance is checked using the distance between the centres of the player and object.
Pressing E while nearby a note will switch the screen to Note and show the loaded text. Gameplay updates are
skipped while the note is open. Enter or Esc returns the player to gameplay. E key interactions also ignore
repeated inputs from holding a key down to prevent accidental inputs while playing a puzzle.

Completion
Once all puzzles are solved the final room opens and contains a pedestal in the centre. The game checks
if the players rectangle is fully inside of the pedestals rectangle, which sets gameCompleted to true and a
completion message is displayed. When the message is closed the game loop ends.

# Expected input handling and test cases:
My game handles expected user input through conditions rather than relying on exceptions. For example, 
movement is clamped to the room boundaries and box pushes are rejected when their destination is impossible.
Missing or invalid asset files are checked when the game starts, if the player sprite cannot be loaded,
it falls back to the default white rectangle so the game can still run and sends an error message.
Notes are also checked when opened and if they cant be read or are missing it will display an error message
in game instead of the note text and in the console. 
I tested these cases alongside normal gameplay:

Test description , Input / condition , Expected result, Actual result
Cant move outside of the room boundaries, Walk into all walls, Player cannot leave, Player could not leave, pass
Sealed doors, Walk into a sealed section of the wall, cannot pass to next room, successfully locked, pass
Boosted move speed on diagonal, Move diagonally, Same move speed consistantly, No faster movement, pass
Light puzzle completion, Correct solition, Door unlocks and puzzle locks, Door opened and stays solved, pass
Invalid box push, Push a box into a wall or other box, Box does not move, Stayed put, pass
Box reset button, Press R in room 2, Boxes return to the start, Puzzle reset properly, pass
Incorrect rune selection, Input wrong order, Lights reset and door stays shut, it reset, pass
Missing Note file, remove a note, error message and game not crashing, expected, pass
Missing Sprite file, remove sprite, error message and fallback, message and player turned into rectangle, pass
Game completion, Stand fully on pedestal in room 4, completion message and exit, worked, pass

I also tested the game running on a computer in the games development lab in uni on thursday november 4th
which successfully ran.

# Use of AI description:
I used AI to support several parts of development. It helped me generate visual assets, including the
rune symbols and treasure chest shapes used in the final room, as well as the symbols on the boxes
and targets in the box puzzle. I incorporated these assets into the game, adjusting their placement and
use where necessary. I also used AI for explanations of SDL3 and C++ concepts, coming up with a few
puzzle ideas although all the puzzle ideas were original as I didnt end up using them and
debugging problems. Any AI assisted code or assets were reviewed, adapted and tested.

# AI development log:

Visual assets:

Coin piles and treasure chest:
Git commit: b5a69b47690abdd8a993fa2cbf6bf8d2c831e2f3
Prompt: draw a 2D chest with sdl and some coin piles to be used in a SDL3 2d game window
Output:

Coins:
void drawCoinPile(SDL_Renderer* renderer, float x, float y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

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

Chest:
void drawTreasureChest(SDL_Renderer* renderer, float x, float y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    const SDL_FRect body{x, y + 20.0f, 64.0f, 32.0f};
    SDL_RenderRect(renderer, &body);

    const SDL_FPoint lid[] = {
        {x,         y + 14.0f},
        {x + 6.0f,  y},
        {x + 58.0f, y},
        {x + 64.0f, y + 14.0f},
        {x,         y + 14.0f}
    };
...

Inside of the final loop i used drawTreasureChest(renderer, 288.0f, 130.0f); and 
drawCoinPile(renderer, 150.0f, 150.0f); to acutally place them down and then I manually made lots of extra
coin piles around the room.

Box symbols:

Prompt: i want to add 4 filled boxes with symbols and a corresponding tile at the end of a grid 
so the left two points and top two points of the hashtag will have boxes, and the bottom two and 
right two will have slots for them to go to

Output:
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

This let me add matching floor slots to the grid aswell as the boxes that get pushed.
GitCommit: 4e7d3a437da5540e0382c5bc8949249946e2c0c5

Idea brainstorming: 

Prompt: Im struggling to come up with an idea for the third puzzle could you give some suggestions
for me to build on please 

Output:
Rotating mirrors
A beam enters the room, and the player rotates mirrors to guide it into a receiver beside the sealed door.
Press E near a mirror to change its direction.
Use horizontal and vertical beams only.
Start with three mirrors and one receiver.
When the beam reaches the receiver, the final door unlocks.
A clue could simply read:
“Lead the captive light to the seal.”
This could make a visually satisfying final puzzle. To keep the 
first version manageable, use mirrors with just two orientations: / and \.
I’d leave splitting beams and multiple receivers for later.

Balance the offering
Place three marked pedestals and several objects with different values. 
The player must arrange the offerings so the pedestals satisfy a clue.
For example:
“The eastern offering weighs twice the western.
The centre must equal them together.”
A possible answer would be west = 2, east = 4, centre = 6.
Press E to pick up or place an offering.
Mark objects with dots or symbols rather than relying on text.
Check the arrangement when the player interacts with the final seal.
This gives you a reasoning puzzle rather than another switch pattern. However, it
introduces carrying objects, so it would be a slightly larger step.

I didn't end up using any of these ideas though but it helped me think about options.

Debugging:
I used it multiple times to help when I ran into errors this is an example:
Prompt:
SeverityCodeDescriptionProjectFileLineSuppression State
ErrorLNK2005"public: void __cdecl BoxPuzzle::reset(void)" (?reset@BoxPuzzle@@QEAAXXZ) already
defined in BoxPuzzle.objTheSpellbreakerC:\Users\Hugo\Documents\GitHub\The Sealbreaker\TheSpellbreaker.obj1

Output:
This means BoxPuzzle::reset() has been defined more than once. The linker is 
finding it in both BoxPuzzle.obj and TheSpellbreaker.obj.

1. Keep only the declaration in BoxPuzzle.h
Inside the class’s public: section, you should have:
void reset();
Don’t put the full void BoxPuzzle::reset() { ... } definition below the class in the header. 
A non-inline function defined in a shared header can cause this exact error.

2. Keep the implementation only in BoxPuzzle.cpp

3. Check your includes

GitCommit: 7f8c4135061a458e1d94cdef5f2de6332b372587

UML design diagram *

Playtesting:

I did two seperate playtesting trials where they both had no idea what the puzzles were going to be originally.

For my first friend Mia, she would have a look throughout me building the puzzles since she enjoys them.
Each time I completed a puzzle she tried it and gave feedback
On 26/10/26 she played the light puzzle.
Feedback: The lights look a bit simple, its quite easy
Changes: Improved their visual style, increased lights from 4 to 6
30/10/26 she played the boz puzzle.
Feedback: more complex than the last and well implemented
1/11/26 she played the rune puzzle.
Feedback: Not too difficult maybe change the clue
Changes: made the clue into a riddle style clue.

For my second friend Baraa he played the full game after it was complete to get a full experience.
Game version: Final build
Date: 5/11/26
Platform: Windows PC
Session Goal: to make sure a player can understand controls and puzzles intuitively
Instructions give: None other than how to launch the game.

Observations:
He read the instructions section of the game before starting, no issues
Toggled crystals before actually checking the note but figured it out with no input, no issues
Pushed the boxes into the correct positions after getting used to pushing them around with little
difficiulty , the boxes edges are sharp so you have to be quite precise when moving them especially
because the player and the boxes are similar size.
Rune puzzle reset properly when incorrectly entered, no issues
After walking around the final room a little, he walked onto the pedestal and won successfully.

Feedback:
Overall easy to understand the puzzles after playing around with them, its nice that you cant ruin the
puzzle and lock yourself without being able to reset. The clues are relatively easy to understand
so I didn't get stuck, though maybe the difficulty could be increased slightly.

Evaluation:
Overall I am quite happy with my work since I haven't worked on anything too similar before so it was a good
new experience to work with SDL3 to make a game in C++. The project delivers the core experience that I
described initially in the proposal. Each puzzle represents a different type of reasoning with light being
logical deduction, the boxes test spacial planning and the rune puzzle tests sequence recognition. This lets
each room have a different feel. Playtesting confirmed that the puzzles I made werent too simple (mostly)
or too difficult which I was glad about as I feel its hard to balance puzzle games especially with no experience.
Developing the game improved my understanding of SDL3 event handling, timing, file input and seperating logic.
Although I met alot of the proposal, there was things that I left out or didn't have time to implement, so I would
possibly work on this again at a later point to flesh it out properly and use proper inheritance so I can expand.

