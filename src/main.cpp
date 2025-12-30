//Based on https://play2048.co/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>

static SDL_Window* Window = NULL;
static SDL_Renderer* Renderer = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("2048 v2.0", "1.0", "Game");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("2048 v2.0", 640, 480, 0, &Window, &Renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(Renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    //Background to white
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

    return SDL_APP_CONTINUE;
}

/* Handeling Events (Keyboard and Mouse Events) */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS; // End of the program
        break;
    case SDL_EVENT_KEY_DOWN: //Inputs of the Game
        switch (event->key.scancode)
        {
        case SDL_SCANCODE_UP:
            SDL_Log("Up Arrow Key Pressed");
            break;
		case SDL_SCANCODE_DOWN:
            SDL_Log("Down Arrow Key Pressed");
            break;
        case SDL_SCANCODE_LEFT:
            SDL_Log("Left Arrow Key Pressed");
			break;
        case SDL_SCANCODE_RIGHT:
            SDL_Log("Right Arrow Key Pressed");
            break;
        }
        break;
    }
    return SDL_APP_CONTINUE;
}

/* Runs each famre (Rendering Stuff Happens) */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	SDL_RenderClear(Renderer);

    SDL_RenderPresent(Renderer);

    return SDL_APP_CONTINUE;
}

/* Runs on shutdown */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{

}