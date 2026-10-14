#include <iostream>
#include <SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
    // Pointer to main window that will everything will be rendered to
    SDL_Window *window = NULL;

    // The graphics renderer contained inside the window
    SDL_Renderer *graphicsRenderer = NULL;

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
    window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); // WINDOW_SHOWN makes sure window is visible after created
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

    // Initialize renderer color SetRenderDrawColor(R, G, B, opaque)
    SDL_SetRenderDrawColor(graphicsRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

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

        // Clear Screen
        SDL_RenderClear(graphicsRenderer);

        // Update Screen
        SDL_RenderPresent(graphicsRenderer);
    }

    // Destroys renderer
    SDL_DestroyRenderer(graphicsRenderer);
    // Destroys the window
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}