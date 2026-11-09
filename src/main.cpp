#include <iostream>
#include <SDL.h>
#include <array>

#include "chip8.h"

const int SCREEN_WIDTH = 64;  // CHIP8 cells across
const int SCREEN_HEIGHT = 32; // CHIP8 cells down
const int SCALE = 10;         // Pixels per cell
const int INSTRUCTIONS_PER_FRAME = 11;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: Chip8 <path-to-rom> " << std::endl;
        return 1;
    }
    // Pointer to main window that will everything will be rendered to
    SDL_Window *window = NULL;

    // The graphics renderer contained inside the window
    SDL_Renderer *graphicsRenderer = NULL;

    // creating the chip8 device going to be used
    Chip8 device;

    // runs load rom method using filepath from main parameter. if loadrom fails, sdl is cleaned up and program is closed
    if (!device.loadRom(argv[1]))
    {
        // Destroys renderer
        SDL_DestroyRenderer(graphicsRenderer);
        // Destroys the window
        SDL_DestroyWindow(window);

        // Quit SDL subsystems
        SDL_Quit();

        return 1;
    }

    // Initializing SDL. If any errors, SDL_Init returns -1
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        // SDL_GetError() prints the error to console whenever SDL_Init goes wrong
        // It returns the latest error produced by an SDL funciton
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Creates window. SDL_CreateWindow(titel, xpos, ypos, width, height, flag)
    // SDL_WINDOWPOS_UNDEFINED lets the OS decide where to put the window in terms of coordinates
    window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN); // WINDOW_SHOWN makes sure window is visible after created
    if (window == NULL)
    {
        // If window isnt created, it stays NULL. Quits the SDL subsystems since it was initialized earlier before ending the program
        std::cerr << "Window could not be created. SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Creates Renderer for the window. Basically tells SDL2 to use GPU instead of CPU to draw graphics
    graphicsRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // If renderer doesnt get created, it quits SDL subsystem and ends program
    if (graphicsRenderer == NULL)
    {
        std::cerr << "Grahics Renderer could not be created: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const auto &display = device.getDisplay();

    // This chunk of code keeps the window open
    // If while quit is false, to see if a nevent occurs and if that event is the "X" button clicked (SDL_QUIT)
    // Otherwise the end of the main function would close the window as well
    SDL_Event e;
    bool quit = false;
    while (quit == false)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
        }
        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++)
        {
            device.step();
        }

        // clearing color to black every frame
        SDL_SetRenderDrawColor(graphicsRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(graphicsRenderer);

        // setting color back to white for drawing purposes
        SDL_SetRenderDrawColor(graphicsRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        for (int y = 0; y < Chip8::DISPLAY_HEIGHT; ++y)
        {
            for (int x = 0; x < Chip8::DISPLAY_WIDTH; ++x)
            {
                int index = x + SCREEN_WIDTH * y;
                if (display[index])
                {
                    SDL_Rect cell;
                    cell.x = x * SCALE;
                    cell.y = y * SCALE;
                    cell.w = SCALE;
                    cell.h = SCALE;
                    SDL_RenderFillRect(graphicsRenderer, &cell);
                }
            }
        }

        // Update Screen
        SDL_RenderPresent(graphicsRenderer);
        SDL_Delay(16);
    }

    // Destroys renderer
    SDL_DestroyRenderer(graphicsRenderer);
    // Destroys the window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}