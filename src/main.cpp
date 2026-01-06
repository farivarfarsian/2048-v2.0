//Based on https://play2048.co/

//TODO:
// // - Add Bomb and Jump features 
// // - Add a Loading Screen
// // - Add a Leaderboard
// // - Added a GameOver screen



//Exclusive for Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
static HWND WindowHandle = NULL;
#endif

//STD Libraries
#include <string>
#include <random>

//SDL
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_mouse.h>

#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

static SDL_Window* Window = NULL;
static SDL_Renderer* Renderer = NULL;

//UI
#define FONT_SIZE 30
TTF_Font* Font = NULL;
SDL_Color Font_Color = { 0, 0, 0, 255 };

static SDL_Texture* Background_Texture = NULL;
static SDL_Texture* Tile_Background_Texture = NULL;

//In Game Variables
enum TileKind
{
    TILE_EMPTY = 0,
    TILE_2 = 2,
    TILE_4 = 4,
    TILE_8 = 8,
    TILE_16 = 16,
    TILE_32 = 32,
    TILE_64 = 64,
    TILE_128 = 128,
    TILE_256 = 256,
    TILE_512 = 512,
    TILE_1024 = 1024,
    TILE_2048 = 2048
};

int Grid[4][4] = { TILE_EMPTY };
int Score = 0;
bool IsGridFull = false;
SDL_FRect TilePosition = { 30.0f, 70.0f, 64.0f * 1.5f, 64.0f * 1.5f };

//Game features
bool JumpAbility = false;

//Preloading the textures for the tiles
static SDL_Texture* _2_Texture = NULL;
static SDL_Texture* _4_Texture = NULL;
static SDL_Texture* _8_Texture = NULL;
static SDL_Texture* _16_Texture = NULL;
static SDL_Texture* _32_Texture = NULL;
static SDL_Texture* _64_Texture = NULL;
static SDL_Texture* _128_Texture = NULL;
static SDL_Texture* _256_Texture = NULL;
static SDL_Texture* _512_Texture = NULL;
static SDL_Texture* _1024_Texture = NULL;
static SDL_Texture* _2048_Texture = NULL;

// Functions for Handeling Moves
void GoUp()
{
    for (int i = 1; i <= 3; i++) // 1 -> 2 -> 3
    {
        for (int j = 0; j < 4; j++)
        {
            if (Grid[i][j] != TILE_EMPTY)
            {
                for (int k = i - 1; k >= 0; k--)
                {
                    if (Grid[k][j] == TILE_EMPTY)
                    {
                        Grid[k][j] = Grid[i][j];
                        Grid[i][j] = TILE_EMPTY;
                        i--;
                    }
                    else
                    {
                        if (Grid[i][j] == Grid[k][j])
                        {
                            if ((i - k) > 1 && JumpAbility == false) //For Avoiding Jumping through Tiles and Merging
                            {

                            }
                            else
                            {
                                Grid[k][j] *= 2;
                                Score += Grid[k][j];
                                Grid[i][j] = TILE_EMPTY;
                            }
                        }
                    }
                }
            }
        }
    }

}
void GoDown()
{
    for (int i = 2; i >= 0; i--) // 2 -> 1 -> 0
    {
        for (int j = 0; j < 4; j++)
        {
            if (Grid[i][j] != TILE_EMPTY)
            {
                for (int k = i + 1; k < 4; k++)
                {
                    if (Grid[k][j] == TILE_EMPTY)
                    {
                        Grid[k][j] = Grid[i][j];
                        Grid[i][j] = TILE_EMPTY;
                        i++;
                    }
                    else
                    {
                        if (Grid[i][j] == Grid[k][j])
                        {
                            if ((k - i) > 1 && JumpAbility == false) //For Avoiding Jumping through Tiles and Merging
                            {

                            }
                            else
                            {
                                Grid[k][j] *= 2;
                                Score += Grid[k][j];
                                Grid[i][j] = TILE_EMPTY;
                            }
                        }
                    }
                }
            }
        }
    }
}   
void GoLeft()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j <= 3; j++) // 1 -> 2 -> 3
        {
            if (Grid[i][j] != TILE_EMPTY)
            {
                for (int k = j - 1; k >= 0; k--)
                {
                    if (Grid[i][k] == TILE_EMPTY)
                    {
                        Grid[i][k] = Grid[i][j];
                        Grid[i][j] = TILE_EMPTY;
                        j--;
                    }
                    else
                    {
                        if (Grid[i][j] == Grid[i][k])
                        {
                            if((j - k) > 1 && JumpAbility == false) //For Avoiding Jumping through Tiles and Merging
                            {

                            }
                            else
                            {
                                Grid[i][k] *= 2;
                                Score += Grid[i][k];
                                Grid[i][j] = TILE_EMPTY;
                            }
                        }
                    }
                }
            }
        }
    }
}
void GoRight()
{
    for(int i  = 0; i < 4; i++)
    {
		for (int j = 2; j >= 0; j--) // 2 -> 1 -> 0
        {
            if(Grid[i][j] != TILE_EMPTY)
            {
                for(int k = j + 1; k < 4; k++)
                {
                    if (Grid[i][k] == TILE_EMPTY)
                    {
                        Grid[i][k] = Grid[i][j];
                        Grid[i][j] = TILE_EMPTY;
                        j++;
                    }
                    else
                    {
                        if (Grid[i][j] == Grid[i][k])
                        {
                            if((k - j) > 1 && JumpAbility == false) //For Avoiding Jumping through Tiles and Merging
                            {
                                
                            }
                            else
                            {
                                Grid[i][k] *= 2;
                                Score += Grid[i][k];
                                Grid[i][j] = TILE_EMPTY;
                            }
                        }
                    }
                }
            }
        }
	}
}

void RespawnTile()
{
#if 0 //Attempt for Dynamic Stack Allocated Array
    std::pair<int, int>* EmptyTiles = static_cast<std::pair<int, int>*>(_alloca(sizeof(std::pair<int, int>) * 16));
    int Index = 0;
#endif
#if 0 // Safe but avoiding Heap Allocations with std::array altough it's with fixed size
    std::array<std::pair<int, int>, 16> EmptyTiles;
#endif

    std::pair<int, int> EmptyTiles[16];
    int Index = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (Grid[i][j] == TILE_EMPTY)
            {
                //EmptyTiles.fill({ i, j });
                EmptyTiles[Index] = { i, j };
                Index++;
            }
        }
    }

    //The Grid is FULLL Checking for if the game has finished
    if (Index == 0)
    {
        IsGridFull = true;

		//Checking for possible moves
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (Grid[i][j] == Grid[i][j + 1]) return;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (Grid[i][j] == Grid[i + 1][j]) return ;
            }
        }

		//No More Moves Available - Game Over
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "2048 v2.0", "Game Over! No More Moves Available!", Window);

        return;
    }
    else
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        int randomIndex = dis(gen);

        Grid[EmptyTiles[randomIndex].first][EmptyTiles[randomIndex].second] = TILE_2;
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata("2048 v2.0", "1.0", "Game");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("2048 v2.0", 576 * 0.8, 778 * 0.7, 0, &Window, &Renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialize TTF library: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    //Loading Font
    Font = TTF_OpenFont("assets/times.ttf", FONT_SIZE);
    if (Font == NULL)
    {
        SDL_Log("Couldn't load the font: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
	TTF_SetFontStyle(Font, TTF_STYLE_BOLD);

    SDL_SetRenderLogicalPresentation(Renderer, 576 * 0.8, 778 * 0.7, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    //Background to white
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

#ifdef _WIN32
    WindowHandle = FindWindowA(NULL, "2048 v2.0");
#endif

    //Load the default textures
    Background_Texture = IMG_LoadTexture(Renderer, "assets/Background.png");
    if (!Background_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }

    //Setting the alpha value for the background texture to 0.8
    if (SDL_SetTextureAlphaModFloat(Background_Texture, 0.8) == false)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Alpha bind failed!\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }

    //
    Tile_Background_Texture = IMG_LoadTexture(Renderer, "assets/Tile_Background.png");
    if (!Tile_Background_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
	}


	// JUST FOR TEST: (DELETE PLS) TODO: Loading Screen
	_2_Texture = IMG_LoadTexture(Renderer, "assets/2.png");
    if(!_2_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
	}
    _4_Texture = IMG_LoadTexture(Renderer, "assets/4.png");
    if (!_4_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _8_Texture = IMG_LoadTexture(Renderer, "assets/8.png");
    if (!_8_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _16_Texture = IMG_LoadTexture(Renderer, "assets/16.png");
    if (!_16_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _32_Texture = IMG_LoadTexture(Renderer, "assets/32.png");
    if (!_32_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _64_Texture = IMG_LoadTexture(Renderer, "assets/64.png");
    if (!_64_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _128_Texture = IMG_LoadTexture(Renderer, "assets/128.png");
    if (!_128_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _256_Texture = IMG_LoadTexture(Renderer, "assets/256.png");
    if (!_128_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _512_Texture = IMG_LoadTexture(Renderer, "assets/512.png");
    if (!_128_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _1024_Texture = IMG_LoadTexture(Renderer, "assets/1024.png");
    if (!_128_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }
    _2048_Texture = IMG_LoadTexture(Renderer, "assets/2048.png");
    if (!_128_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }

    //Create entities
    Grid[0][0] = TILE_2;
    Grid[3][3] = TILE_2;

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
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        float x, y;
        SDL_GetMouseState(&x, &y);
        SDL_FPoint MousePoint;
		MousePoint.x = x;
		MousePoint.y = y;
        if (SDL_PointInRectFloat(&MousePoint, &TilePosition))
        {
            SDL_Log("Mouse Button Pressed inside the Tile at (%.2f, %.2f)", event->button.x, event->button.y);
        }
        else
        {
            SDL_Log("Mouse Button Pressed outside the Tile at (%.2f, %.2f)", event->button.x, event->button.y);
        }
		break;
    case SDL_EVENT_KEY_DOWN: //Inputs of the Game
        switch (event->key.scancode)
        {
        case SDL_SCANCODE_UP:          
            GoUp();
            RespawnTile();
            SDL_Log("Up Arrow Key Pressed");
            break;
		case SDL_SCANCODE_DOWN:
            GoDown();
            RespawnTile();
            SDL_Log("Down Arrow Key Pressed");
            break;
        case SDL_SCANCODE_LEFT:
			GoLeft();
            RespawnTile();
            SDL_Log("Left Arrow Key Pressed");
			break;
        case SDL_SCANCODE_RIGHT:
			GoRight();
            RespawnTile();
            SDL_Log("Right Arrow Key Pressed");
            break;
        case SDL_SCANCODE_J:
            JumpAbility = !JumpAbility;
            if (JumpAbility)
            {
                SDL_Log("Jump Ability Enabled");
            }
            else
            {
                SDL_Log("Jump Ability Disabled");
            }
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

    //Rendering stuff comes here
    
    //Rendering the Background
    SDL_RenderTexture(Renderer, Background_Texture, NULL, NULL);

    //Rendering the Grid
    for(int i = 0; i < 4; i++)
    {
        SDL_FRect destRect1 = { 30.0f, 70.0f + (i * 100.0f), 64.0f * 1.5f, 64.0f * 1.5f};
        SDL_RenderTexture(Renderer, Tile_Background_Texture, NULL, &destRect1);

        SDL_FRect destRect2 = { 130.0f, 70.0f + (i * 100.0f), 64.0f * 1.5f, 64.0f * 1.5f };
        SDL_RenderTexture(Renderer, Tile_Background_Texture, NULL, &destRect2);

        SDL_FRect destRect3 = { 230.0f, 70.0f + (i * 100.0f), 64.0f * 1.5f, 64.0f * 1.5f };
        SDL_RenderTexture(Renderer, Tile_Background_Texture, NULL, &destRect3);

        SDL_FRect destRect4 = { 330.0f, 70.0f + (i * 100.0f), 64.0f * 1.5f, 64.0f * 1.5f };
        SDL_RenderTexture(Renderer, Tile_Background_Texture, NULL, &destRect4);
	}

    //Rendering the Tiles
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            switch (Grid[i][j])
            {
            case TILE_2:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _2_Texture, NULL, &TilePosition);
                break;
            case TILE_4:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _4_Texture, NULL, &TilePosition);
                break;
            case TILE_8:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _8_Texture, NULL, &TilePosition);
                break;
			case TILE_16:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _16_Texture, NULL, &TilePosition);
                break;
			case TILE_32:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _32_Texture, NULL, &TilePosition);
                break;
            case TILE_64:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _64_Texture, NULL, &TilePosition);
                break;
			case TILE_128:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _128_Texture, NULL, &TilePosition);
                break;
            case TILE_256:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _256_Texture, NULL, &TilePosition);
                break;
            case TILE_512:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _512_Texture, NULL, &TilePosition);
				break;
            case TILE_1024:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _1024_Texture, NULL, &TilePosition);
				break;
            case TILE_2048:
                TilePosition.x = 30.0f + (j * 100.0f);
                TilePosition.y = 70.0f + (i * 100.0f);
                SDL_RenderTexture(Renderer, _2048_Texture, NULL, &TilePosition);
                break;
            }
        }
    }

    /// JUST FOR TEST: (DELETE PLS)
    SDL_Surface* surface = TTF_RenderText_Blended(Font, ("Score: " + std::to_string(Score)).c_str(), 0, Font_Color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer, surface);
    SDL_FRect destRect = { 20.0f, 20.0f, (float)surface->w, (float)surface->h };
    SDL_RenderTexture(Renderer, texture, NULL, &destRect);

    SDL_RenderPresent(Renderer);


    /// JUST FOR TEST: (DELETE PLS)
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);

    return SDL_APP_CONTINUE;
}

/* Runs on shutdown */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    TTF_CloseFont(Font);
    TTF_Quit();
}