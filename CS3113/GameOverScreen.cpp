#include "GameOverScreen.h"

GameOverScreen::GameOverScreen() : Scene{{0.0f}, nullptr} {}
GameOverScreen::GameOverScreen(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}

GameOverScreen::~GameOverScreen() { shutdown(); }

void GameOverScreen::initialise()
{
    mGameState.nextSceneID = 0;
}

void GameOverScreen::update(float deltaTime)
{
}

void GameOverScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("You Have Lost!", 600, 400, 20, BLUE);
    DrawText("Press Enter to Try Again from the Start", 600, 420, 20, RED);
}

void GameOverScreen::shutdown()
{
}