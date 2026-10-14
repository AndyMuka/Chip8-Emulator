#include <iostream>
#include <SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[])
{
    // Pointer to main window that will everything will be rendered to
    SDL_Window *window = NULL;

    // The surface contained inside the window
    SDL_Surface *screenSurface = NULL;

    // Initializing SDL. If any errors, SDL_Init returns -1
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        // SDL_GetError() prints the error to console whenever SDL_Init goes wrong
        // It returns the latest error produced by an SDL funciton
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    else
    {
        // Creates window. SDL_CreateWindow(titel, xpos, ypos, width, height, flag)
        // SDL_WINDOWPOS_UNDEFINED lets the OS decide where to put the window in terms of coordinates
        window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); // WINDOW_SHOWN makes sure window is visible after created
        if (window == NULL)
        {
            // If window isnt created, it stays NULL.
            std::cout << "Window could not be created. SDL_Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            // Creates a real screen and attaches it to the screenSurface variable
            screenSurface = SDL_GetWindowSurface(window);

            // Fills the surface white (draws on surface)
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

            // Update the surface (makes drawing actually visible)
            SDL_UpdateWindowSurface(window);

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
            }
        }
    }
}