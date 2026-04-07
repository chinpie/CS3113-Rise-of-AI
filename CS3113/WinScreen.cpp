#include "WinScreen.h"

WinScreen::WinScreen() : Scene{{0.0f}, nullptr} {}
WinScreen::WinScreen(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}

WinScreen::~WinScreen() { shutdown(); }

void WinScreen::initialise()
{
    mGameState.nextSceneID = 0;
}

void WinScreen::update(float deltaTime)
{
}

void WinScreen::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("You Have WON!", 600, 400, 20, BLUE);
    DrawText("Press Enter to Try Again from the Start", 600, 420, 20, RED);
    DrawText("Credits:\n Jonathan Guy (Amazing Amazing Music)", 600, 440, 20, RED);
}

void WinScreen::shutdown()
{
}