#include "MenuScreen.h"

MenuScreen::MenuScreen() : Scene{{0.0f}, nullptr} {}
MenuScreen::MenuScreen(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}

MenuScreen::~MenuScreen() { shutdown(); }

void MenuScreen::initialise()
{
    mGameState.nextSceneID = 0;
}

void MenuScreen::update(float deltaTime)
{
}

void MenuScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("CLICK ENTER TO START", 600, 500, 20, BLUE);
}

void MenuScreen::shutdown()
{
}