/**
 * Author: Tyler Davidovich
 * Assignment: Rise of the AI
 * Date due: 2025-11-04, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/WinScreen.h"
#include <string>

// Enums`

// Global Constants
constexpr int SCREEN_WIDTH = 1500,
              SCREEN_HEIGHT = 900,
              FPS = 120,
              NUMBER_OF_LEVELS = 7;

constexpr char BG_COLOUR[] = "#C0897E";
constexpr Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
                  ATLAS_DIMENSIONS = {2, 5},
                  DIGLETT_DIMENSIONS = {8, 2};

constexpr int NUMBER_OF_TILES = 20,
              NUMBER_OF_BLOCKS = 3;
constexpr float TILE_DIMENSION = 75.0f,
                // in m/ms², since delta time is in ms
    ACCELERATION_OF_GRAVITY = 500.0f,
                FIXED_TIMESTEP = 1.0f / 60.0f,
                END_GAME_THRESHOLD = 800.0f;

// Global Variables
Scene *gCurrentScene = nullptr;
std::vector<Scene *> gLevels = {};
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;
LevelD *gLevelD = nullptr;
MenuScreen *gMenuScreen = nullptr;
WinScreen *gWinScreen = nullptr;
GameOverScreen *gGameOverScreen = nullptr;

AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f,
      gTimeAccumulator = 0.0f,
      startThrow = GetTime() - 999.0f;
Camera2D gCamera = {0};

GameState gState;

// Function Declarations
void switchToScene(Scene *scene, int lifeCount);
void initialise();
void processInput();
void update();
void render();
void shutdown();
void panCamera(Camera2D *camera, const Vector2 *targetPosition);

/**
 * The function `panCamera` smoothly adjusts the camera's target position towards a specified target
 * position.
 *
 * @param camera The `camera` parameter is a pointer to a `Camera2D` struct, which likely contains
 * information about a 2D camera such as its position, target, zoom level, and other properties.
 * @param targetPosition The `targetPosition` parameter is a pointer to a `Vector2` structure that
 * represents the desired position where the camera should be focused or centered on.
 */
void panCamera(Camera2D *camera, const Vector2 *targetPosition)
{
    Vector2 positionDifference = Vector2Subtract(
        *targetPosition,
        camera->target);

    camera->target = Vector2Add(
        camera->target,
        Vector2Scale(positionDifference, 0.1f)); // 0.1 = smoothing factor
}

void switchToScene(Scene *scene, int lifeCount)
{
    gCurrentScene = scene;
    gCurrentScene->setLife(lifeCount);
    gCurrentScene->initialise();
    if (gCurrentScene->getState().pokeTrainer != nullptr)
    {
        gCamera.target = gCurrentScene->getState().pokeTrainer->getPosition();
    }
    else
    {
        gCamera.target = ORIGIN;
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Attack of the Wild Pokemon");
    InitAudioDevice();
    gMenuScreen = new MenuScreen(ORIGIN, "#27e1cf");
    gGameOverScreen = new GameOverScreen(ORIGIN, "#000000");
    gWinScreen = new WinScreen(ORIGIN, "#eeff02");
    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#4f1206");
    gLevelC = new LevelC(ORIGIN, "#d6cbc9");
    gLevelD = new LevelD(ORIGIN, "#c2dc1d8b");

    gLevels.push_back(gMenuScreen);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);
    gLevels.push_back(gLevelD);
    gLevels.push_back(gGameOverScreen);
    gLevels.push_back(gWinScreen);

    switchToScene(gLevels[0], 3);

    gCamera.offset = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 2.0f;

    SetTargetFPS(FPS);
}

void processInput()
{
    if (gCurrentScene->getState().pokeTrainer != nullptr)
    {
        gCurrentScene->getState().pokeTrainer->resetMovement();

        if (IsKeyDown(KEY_A))
            gCurrentScene->getState().pokeTrainer->moveLeft();
        else if (IsKeyDown(KEY_D))
            gCurrentScene->getState().pokeTrainer->moveRight();

        if (IsKeyPressed(KEY_W) &&
            gCurrentScene->getState().pokeTrainer->isCollidingBottom())
        {
            gCurrentScene->getState().pokeTrainer->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(gCurrentScene->getState().pokeTrainer->getMovement()) > 1.0f)
            gCurrentScene->getState().pokeTrainer->normaliseMovement();
    }
    if (IsKeyPressed(KEY_Q) || WindowShouldClose())
        gAppStatus = TERMINATED;

    if (IsKeyPressed(KEY_E) && (GetTime() - startThrow >= 1.0f))
    {
        gCurrentScene->throwPokeBall();
        startThrow = GetTime();
    }
    if ((gCurrentScene == gMenuScreen || gCurrentScene == gGameOverScreen || gCurrentScene == gWinScreen) && IsKeyPressed(KEY_ENTER))
    {
        if (gCurrentScene->getState().pokeTrainer != nullptr)
        {
            gCurrentScene->getState().pokeTrainer->setPlayerLives(3);
        }
        gCurrentScene->changeScene(1);
    }
    if (IsKeyPressed(KEY_ONE))
    {
        switchToScene(gLevels[0], 3);
    }
    if (IsKeyPressed(KEY_TWO))
    {
        gCurrentScene->changeScene(1);
    }
    if (IsKeyPressed(KEY_THREE))
    {
        gCurrentScene->changeScene(2);
    }
    if (IsKeyPressed(KEY_FOUR))
    {
        gCurrentScene->changeScene(3);
    }
    if (IsKeyPressed(KEY_FIVE))
    {
        gCurrentScene->changeScene(4);
    }
    if (IsKeyPressed(KEY_SIX))
    {
        gCurrentScene->changeScene(5);
    }
    if (IsKeyPressed(KEY_SEVEN))
    {
        gCurrentScene->changeScene(6);
    }
}

void update()
{
    // Delta time
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
        if (gCurrentScene->getState().pokeTrainer != nullptr)
        {
            Vector2 currentPlayerPosition = {gCurrentScene->getState().pokeTrainer->getPosition().x, gCurrentScene->getState().pokeTrainer->getPosition().y};
            panCamera(&gCamera, &currentPlayerPosition);

            if (gCurrentScene->getState().pokeTrainer->getPlayerLives() <= 0)
            {
                switchToScene(gLevels[5], 3);
                return;
            }
        }

        {
        }
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);
    gCurrentScene->render();
    EndMode2D();
    std::string curPokeBalls = "Total PokeBalls Remaining: " + std::to_string(gCurrentScene->getState().numPokeBalls);
    if (gCurrentScene != gMenuScreen && gCurrentScene != gLevelD)
    {
        DrawText(curPokeBalls.c_str(), 100.0f, 100.0f, 40, RED);
    }
    else if (gCurrentScene == gLevelD)
    {
        DrawText("SURVIVE!\nDo not be on the floor when it lands...", 100.0f, 100.0f, 40, RED);
    }

    EndDrawing();
}

void shutdown()
{
    delete gMenuScreen;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;
    delete gLevelD;
    delete gWinScreen;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++)
        gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            int currentLife = gCurrentScene->getState().playerLifeCount;
            switchToScene(gLevels[id], currentLife);
        }
        render();
    }

    shutdown();

    return 0;
}