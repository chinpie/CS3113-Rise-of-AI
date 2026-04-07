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
    DrawText("CLICK ENTER TO START", 600, 400, 20, BLUE);
    DrawText("Wild Pokemon Have Escaped!\nYour goal is to catch them.", 600, 420, 20, RED);
    DrawText("\nPress [E] to throw a pokeball.\nBeware you have a limited Amount!", 600, 440, 20, RED);
}

void MenuScreen::shutdown()
{
}