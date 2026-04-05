/**
 * Author: Tyler Davidovich
 * Assignment: Rise of the AI
 * Date due: 2025-11-04, 11:59pm
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

#include "CS3113/LevelC.h"

// Enums`

// Global Constants
constexpr int SCREEN_WIDTH = 1500,
              SCREEN_HEIGHT = 900,
              FPS = 120,
              NUMBER_OF_LEVELS = 3;

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
MenuScreen *gMenuScreen = nullptr;

AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f,
      gTimeAccumulator = 0.0f;
Camera2D gCamera = {0};

GameState gState;

// Function Declarations
void switchToScene(Scene *scene);
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

void switchToScene(Scene *scene)
{
    gCurrentScene = scene;
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Maps");
    InitAudioDevice();
    gMenuScreen = new MenuScreen(ORIGIN, "#C0897E");
    gLevelA = new LevelA(ORIGIN, "#C0897E");
    gLevelB = new LevelB(ORIGIN, "#4f1206");
    gLevelC = new LevelC(ORIGIN, "#d6cbc9");
    gLevels.push_back(gMenuScreen);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gLevels[0]);

    gCamera.offset = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 3.5f;

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

    if (IsKeyPressed(KEY_E))
    {
        gCurrentScene->throwPokeBall();
    }
    if (gCurrentScene == gMenuScreen && IsKeyPressed(KEY_ENTER))
    {
        gCurrentScene->changeScene(1);
    }
    if (IsKeyPressed(KEY_ONE))
    {
        gCurrentScene = gMenuScreen;
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
        }
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);
    gCurrentScene->render();
    EndMode2D();

    EndDrawing();
}

void shutdown()
{
    delete gMenuScreen;
    delete gLevelA;
    delete gLevelB;
    delete gLevelC;

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
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}