//Based on https://play2048.co/

//TODO:
// // - Paging for Leaderboard

//Exclusive for Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
static HWND WindowHandle = NULL;
#endif

//TinyXML Library
#include <tinyxml2.h>
using namespace tinyxml2;

//STD Libraries
#include <string>
#include <random>
#include <filesystem>
#include <vector>

// for running a pieace of code only once
bool Flag = 0 , Flag1 = 0, Flag2 = 0;

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
#define WINDOW_WIDTH (576 * 0.8)
#define WINDOW_HEIGHT (778 * 0.7)

// HUD
#define FONT_SIZE 30
TTF_Font* Font = NULL;
SDL_Color Font_Color = { 0, 0, 0, 255 };

static SDL_Texture* Background_Texture = nullptr;
static SDL_Texture* MainMenu_Texture = nullptr;
static SDL_Texture* Tile_Background_Texture = nullptr;
static SDL_Texture* BottomUI = nullptr;

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
std::string InputUserText;
int Score = 0;
bool UserIsTyping = false;

// Handeling in-game events (for a better and cleaner code building an event or layer system is recommended)
bool IsScoreChanged = false;
bool IsGameOver = false;
bool IsMainMenuOn = true;
bool IsLeaderboardOn = false;
bool IsBackPressed = false;
bool IsGridFull = false;

SDL_FRect TilePosition = { 30.0f, 70.0f, 64.0f * 1.5f, 64.0f * 1.5f };
// Buttons
SDL_FRect Play_Button, Leaderboard_Button, Back_Button;

// Wrapper Functions and Variables
SDL_Texture* ScoreTextWrapper = nullptr;
SDL_Texture* UserInputTextureWrapper = nullptr;
SDL_Texture* GameOverTextureWrapper = nullptr;
SDL_Texture* MainMenu_Play_TextureWrapper = nullptr;
SDL_Texture* MainMenu_Leaderboard_TextureWrapper = nullptr;

//Leaderboard
XMLDocument Document;
XMLElement* Leaderboard = nullptr;
XMLElement* CurrentUser = nullptr;
std::vector<std::string> UsernamesScores;

//Game features
bool JumpAbility = false;
bool AutoSave = true;
void SaveTheGame()
{
	// Improvements for this could be using a hash map to store users for O(1) access time
    

	// Iterating through XML to find if user exists
    bool UserExists = false;
    for (XMLElement* userElem = Document.FirstChildElement()->FirstChildElement("User");
        userElem != nullptr;
        userElem = userElem->NextSiblingElement("User"))
    {
        std::string username = "default";

        const XMLAttribute* attr = userElem->FirstAttribute();
        if (attr) 
        {
            username = attr->Name();
        }
        if(username == InputUserText)
        {
            userElem->FirstChildElement("Score")->SetText(Score);
            UserExists = true;
            break;
		}
    }

    if (UserExists == false)
    {
        CurrentUser = Document.NewElement("User");
        CurrentUser->SetAttribute(InputUserText.c_str(), 1);
        Document.FirstChildElement()->InsertEndChild(CurrentUser);

        XMLElement* ScoreElement = Document.NewElement("Score");
        ScoreElement->SetText(Score);
        CurrentUser->InsertEndChild(ScoreElement);
    }

    Document.SaveFile("Leaderboard.xml");
}

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

// Scenes
// the better method to implement this would be with a class and ESC and using OOP after all, but for simplicity implementation is c like
// 
// 2048 Game
void _2048GameScene()
{
    //Rendering the Grid
    for (int i = 0; i < 4; i++)
    {
        SDL_FRect destRect1 = { 30.0f, 70.0f + (i * 100.0f), 64.0f * 1.5f, 64.0f * 1.5f };
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

    //Getting the Username from input
    //std::call_once(flag, UserIsTypingWrapperTrue);
    if (Flag == 0)
    {
        UserIsTyping = true;
        Flag = 1;
    }

    if (UserIsTyping == true)
    {
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 180);
        SDL_RenderFillRect(Renderer, NULL);

        SDL_StartTextInput(Window);

        SDL_FRect DestRect;
        SDL_Surface* Surface = nullptr;
        Surface = TTF_RenderText_Blended(Font, "Enter your Username: ", 0, Font_Color);
        UserInputTextureWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
        DestRect = { (WINDOW_WIDTH / 2) - 150, (WINDOW_HEIGHT / 2) - 60, (float)Surface->w, (float)Surface->h };

        SDL_RenderTexture(Renderer, UserInputTextureWrapper, NULL, &DestRect);

        //Cleaning Up for UserInput Stuff
        SDL_DestroySurface(Surface);
        SDL_DestroyTexture(UserInputTextureWrapper);

        if (InputUserText.empty() == false)
        {
            SDL_Surface* Surface = TTF_RenderText_Blended(Font, InputUserText.c_str(), 0, Font_Color);
            UserInputTextureWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
            if (InputUserText.length() > 12)
            {
                DestRect = { (WINDOW_WIDTH / 2) - 150, (WINDOW_HEIGHT / 2) - 30, (float)Surface->w, (float)Surface->h };
            }
            else
            {
                DestRect = { (WINDOW_WIDTH / 2) - 50, (WINDOW_HEIGHT / 2) - 30, (float)Surface->w, (float)Surface->h };
            }
            SDL_DestroySurface(Surface);
            SDL_RenderTexture(Renderer, UserInputTextureWrapper, NULL, &DestRect);
        }
    }
}
void _2048GameUIScene()
{
    //Rendering the Background
    SDL_RenderTexture(Renderer, Background_Texture, NULL, NULL);

    SDL_FRect DestRect = { 30.0f, 471.0f, 576 * 0.7, 128 * 0.6 };
    SDL_RenderTexture(Renderer, BottomUI, NULL, &DestRect);

    if (IsScoreChanged == true)
    {
        SDL_Surface* Surface = TTF_RenderText_Blended(Font, ("Score: " + std::to_string(Score)).c_str(), 0, Font_Color);
        ScoreTextWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
        DestRect = { 20.0f, 20.0f, (float)Surface->w, (float)Surface->h };
        SDL_DestroySurface(Surface);
        SDL_RenderTexture(Renderer, ScoreTextWrapper, NULL, &DestRect);
    }
}
// Main Menu
void MainMenuScene()
{
    SDL_FRect DestRect;
    DestRect = { 0.0f, 0.0f, 576, 900 };

    SDL_RenderTexture(Renderer, MainMenu_Texture, NULL, &DestRect);

    SDL_Surface* Surface = nullptr;
    Surface = TTF_RenderText_Blended(Font, "Play", 0, { 255, 255, 0, 255 });
    MainMenu_Play_TextureWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
    Play_Button = { (WINDOW_WIDTH / 2) - 30, (WINDOW_HEIGHT / 2) - 75, (float)Surface->w, (float)Surface->h };
    SDL_DestroySurface(Surface);
    SDL_RenderTexture(Renderer, MainMenu_Play_TextureWrapper, NULL, &Play_Button);

    Surface = TTF_RenderText_Blended(Font, "Leaderboard", 0, { 0, 0, 255, 255 });
    MainMenu_Leaderboard_TextureWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
    Leaderboard_Button = { (WINDOW_WIDTH / 2) - 90, (WINDOW_HEIGHT / 2) + 15, (float)Surface->w, (float)Surface->h };
    SDL_DestroySurface(Surface);

    SDL_RenderTexture(Renderer, MainMenu_Leaderboard_TextureWrapper, NULL, &Leaderboard_Button);
}
// Game Over
void GameOverScene()
{
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(Renderer, NULL);

    SDL_FRect DestRect;
    SDL_Surface* Surface = nullptr;
    Surface = TTF_RenderText_Blended(Font, "GAME OVER !", 0, { 255, 0, 0, 255 });
    GameOverTextureWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
    DestRect = { (WINDOW_WIDTH / 2) - 120, (WINDOW_HEIGHT / 2) - 60, (float)Surface->w, (float)Surface->h };

    SDL_RenderTexture(Renderer, GameOverTextureWrapper, NULL, &DestRect);

    Surface = TTF_RenderText_Blended(Font, "Press any key to continue", 0, { 0, 255, 0, 255 });
    GameOverTextureWrapper = SDL_CreateTextureFromSurface(Renderer, Surface);
    DestRect = { (WINDOW_WIDTH / 2) - 200, (WINDOW_HEIGHT / 2) - 20, (float)Surface->w, (float)Surface->h };


    SDL_RenderTexture(Renderer, GameOverTextureWrapper, NULL, &DestRect);

    SDL_DestroySurface(Surface);

    // Clearing old data
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Grid[i][j] = TILE_EMPTY;
        }
    }
    Score = 0;
    InputUserText.clear();
    Flag = 0, Flag1 = 0, Flag2 = 0;

    Grid[0][0] = TILE_2;
    Grid[3][3] = TILE_2;

    // Going Back to Main Menu ... 
}
// Leaderboard
void LeaderboardScene()
{
    SDL_FRect DestRect;
    DestRect = { 0.0f, 0.0f, 576, 900 };

    SDL_RenderTexture(Renderer, MainMenu_Texture, NULL, &DestRect);

    //Render them as text on screen
    SDL_Surface* Surface = TTF_RenderText_Blended(Font, "Leaderboard: ", 0, { 0,0,255,255 });
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    SDL_FRect destRect = { 30.0f, 10.0f, (float)Surface->w, (float)Surface->h };

    SDL_RenderTexture(Renderer, Texture, NULL, &destRect);

    SDL_DestroySurface(Surface);

    SDL_DestroyTexture(Texture);


    //Render them as text on screen
    Surface = TTF_RenderText_Blended(Font, "Back...", 0, { 255,0,0,255 });
    Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
    Back_Button = { 30.0f, 500.0f, (float)Surface->w, (float)Surface->h };

    SDL_RenderTexture(Renderer, Texture, NULL, &Back_Button);

    SDL_DestroySurface(Surface);

    SDL_DestroyTexture(Texture);

    if (Flag2 == 0)
    {
        XMLElement* UserElememt = Document.FirstChildElement()->FirstChildElement("User");
        std::string LastUsername;

        while (UserElememt != nullptr)
        {

            std::string Username;

            //Retrieving Username and Score
            const XMLAttribute* Name = UserElememt->FirstAttribute();
            XMLElement* ScoreElement = nullptr;
            if (Name)
            {
                Username = Name->Name();
                if (LastUsername.empty() == false)
                {
                    if (LastUsername == Username)
                    {
                        goto AvoidDup; // Avoiding Duplicates
                    }
                    else
                    {
                        LastUsername = Username;
                    }
                }

            }

            ScoreElement = UserElememt->FirstChildElement("Score");
            if (ScoreElement)
            {
                Score = ScoreElement->IntText();
            }

            UsernamesScores.push_back((Username + ": " + std::to_string(Score)));

        AvoidDup:
            UserElememt = UserElememt->NextSiblingElement("User");

        }
        if (UserElememt == nullptr)
        {
            Flag2 = 1;
        }
    }

    for (int i = 0; i < UsernamesScores.size(); i++)
    {
        //Render them as text on screen
        SDL_Surface* Surface = TTF_RenderText_Blended(Font, UsernamesScores[i].c_str(), 0, Font_Color);
        SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
        SDL_FRect destRect = { 50.0f, 50.0f + (i * 40.0f), (float)Surface->w, (float)Surface->h };

        SDL_RenderTexture(Renderer, Texture, NULL, &destRect);

        SDL_DestroySurface(Surface);

        SDL_DestroyTexture(Texture);
    }
}

// Scene Killers
// 
// 2048 Game
void _2048KillGameScene()
{
    SDL_DestroyTexture(Tile_Background_Texture);

    SDL_DestroyTexture(_2_Texture);
    SDL_DestroyTexture(_4_Texture);
    SDL_DestroyTexture(_8_Texture);
    SDL_DestroyTexture(_16_Texture);
    SDL_DestroyTexture(_32_Texture);
    SDL_DestroyTexture(_64_Texture);
    SDL_DestroyTexture(_128_Texture);
    SDL_DestroyTexture(_256_Texture);
    SDL_DestroyTexture(_512_Texture);
    SDL_DestroyTexture(_1024_Texture);
    SDL_DestroyTexture(_2048_Texture);

    SDL_DestroyTexture(BottomUI);
}
void _2048KillGameUIScene()
{
    SDL_DestroyTexture(Background_Texture);
    SDL_DestroyTexture(MainMenu_Texture);
}
void KillLeaderboardScene()
{
    //Reseting values
    IsLeaderboardOn = false;
    IsBackPressed = false;
    UsernamesScores.clear();
    Score = 0;
    Flag2 = 0;
}

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
                                IsScoreChanged = true;
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
                                IsScoreChanged = true;
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
                                IsScoreChanged = true;
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
                                IsScoreChanged = true;
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

        // Insert Username + Score to XML
        // 
        // 
        //
        //std::call_once(flag1, SaveTheGame); 
        if (Flag1 == 0)
        {
            SaveTheGame();
            Flag1 = 1;
        }

        
        //No More Moves Available - Game Over
        IsGameOver = true;

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

    MainMenu_Texture = IMG_LoadTexture(Renderer, "assets/Background2.png");
    if (!MainMenu_Texture)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load texture\nExiting the game", Window);
        return SDL_APP_FAILURE;
    }

    BottomUI = IMG_LoadTexture(Renderer, "assets/BottomUI.png");
    if (!BottomUI)
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

    // Leaderboard file
    if (std::filesystem::exists("Leaderboard.xml") == true)
    {
        if (Document.LoadFile("Leaderboard.xml") != tinyxml2::XML_SUCCESS)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't load the ""Leaderboard.xml"" file\nExiting the game", Window);
            return SDL_APP_FAILURE;
        }
    }
    else
    {
        //Creating the file
        Leaderboard = Document.NewElement("Leaderboard");
        Document.InsertFirstChild(Leaderboard);
        if (Document.SaveFile("Leaderboard.xml") != 0)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "2048 v2.0", "Couldn't write to ""Leaderboard.xml"" file\nExiting the game", Window);
            return SDL_APP_FAILURE;
        }
    }


	// TODO: Loading Screen
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
    //Could Implent it to be random but what's the point ?!
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
        if (AutoSave == true)
        {
            if (InputUserText.empty() == true)
            {
                InputUserText = "default";
            }
            SaveTheGame();
        }
        return SDL_APP_SUCCESS; // End of the program
        break;
    case SDL_EVENT_TEXT_INPUT:
        InputUserText += event->text.text;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN: 
        float x, y;
        SDL_GetMouseState(&x, &y);
        SDL_FPoint MousePoint;
        MousePoint.x = x;
        MousePoint.y = y;
        if (IsMainMenuOn == true)
        {
            if (SDL_PointInRectFloat(&MousePoint, &Play_Button))
            {
                IsMainMenuOn = false;
                SDL_Delay(100);
            }
            else if (SDL_PointInRectFloat(&MousePoint, &Leaderboard_Button))
            {
                IsLeaderboardOn = true;
                SDL_Delay(100);
            }
            else if (SDL_PointInRectFloat(&MousePoint, &Back_Button))
            {
                IsBackPressed = true;
                SDL_Delay(100);
            }
        }
#if 0 // For the BOMB in-game feature but it's cut due to time issues
        if (UserIsTyping == false)
        {
            float x, y;
            SDL_GetMouseState(&x, &y);
            SDL_FPoint MousePoint;
            MousePoint.x = x;
            MousePoint.y = y;
            if (SDL_PointInRectFloat(&MousePoint, &TilePosition))
            {
                SDL_Log("Mouse Button Pressed inside the Tile at (%.2f, %.2f)", event->button.x, event->button.y);
                //Explode (Clear) All the tiles in the same column and row of the selected tile
            }
            else
            {
                SDL_Log("Mouse Button Pressed outside the Tile at (%.2f, %.2f)", event->button.x, event->button.y);
            }
        }
#endif
		break;
    case SDL_EVENT_KEY_DOWN: //Inputs of the Game
        if (IsGameOver == true)
        {
            IsGameOver = false;
            IsMainMenuOn = true;
        }
        else
        {
            switch (event->key.scancode)
            {
            case SDL_SCANCODE_BACKSPACE:
                if (!InputUserText.empty())
                {
                    InputUserText.pop_back();
                }
                break;
            case SDL_SCANCODE_RETURN:
                if (InputUserText.empty() == false)
                {
                    InputUserText.erase(remove(InputUserText.begin(), InputUserText.end(), ' '), InputUserText.end()); //Removing Spaces
                    SDL_StopTextInput(Window);
                    UserIsTyping = false;
                }
                break;
            case SDL_SCANCODE_UP:
                if (UserIsTyping == false)
                {
                    GoUp();
                    RespawnTile();
                }
                break;
            case SDL_SCANCODE_DOWN:
                if (UserIsTyping == false)
                {
                    GoDown();
                    RespawnTile();
                }
                break;
            case SDL_SCANCODE_LEFT:
                if (UserIsTyping == false)
                {
                    GoLeft();
                    RespawnTile();
                }
                break;
            case SDL_SCANCODE_RIGHT:
                if (UserIsTyping == false)
                {
                    GoRight();
                    RespawnTile();
                }
                break;
            case SDL_SCANCODE_J:
                if (UserIsTyping == false)
                {
                    JumpAbility = !JumpAbility; // For noob players :(
                    if (JumpAbility)
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "2048 v2.0", "Jump Ability Enabled\nDon't abuse it :)", Window);
                    }
                    else
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "2048 v2.0", "Jump Ability Disabled\nThis is the right choice ;)", Window);
                    }
                }
                break;
            case SDL_SCANCODE_S:
                if (UserIsTyping == false && AutoSave == false)
                {
                    SaveTheGame();
                }
                break;
            }
            break;
        }
        }
    return SDL_APP_CONTINUE;
}

/* Runs each famre (Rendering Stuff Happens) */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	SDL_RenderClear(Renderer);

    // Scenes
    if (IsMainMenuOn == true)
    {
        if (IsLeaderboardOn ==  false)
        {
            MainMenuScene();
        }
        else
        {
            if (IsBackPressed == false)
            {
                LeaderboardScene();
            }
            else
            {
				KillLeaderboardScene();
            }
        }
    }
    else
    {
        if (IsGameOver == false)
        {
            _2048GameUIScene();
            _2048GameScene();
        }
        else
        {
            GameOverScene();
        }
    }

    SDL_RenderPresent(Renderer);

    //Deleting Resource to avoid Memory Leaking
    SDL_DestroyTexture(ScoreTextWrapper);
    SDL_DestroyTexture(UserInputTextureWrapper);
    SDL_DestroyTexture(GameOverTextureWrapper);
    SDL_DestroyTexture(MainMenu_Play_TextureWrapper);
    SDL_DestroyTexture(MainMenu_Leaderboard_TextureWrapper);

    return SDL_APP_CONTINUE;
}

/* Runs on shutdown */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    TTF_CloseFont(Font);
    TTF_Quit();

    _2048KillGameScene();
    _2048KillGameUIScene();

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
}