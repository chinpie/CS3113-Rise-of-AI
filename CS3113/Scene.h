#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *pokeTrainer;
    std::vector<Entity *> pokeBalls;
    Map *map;
    Entity *diglett;
    Entity *pidgey;
    Entity *samurott;
    Music bgm;
    Sound jumpSound;
    Entity *BG1;
    Entity *pokeBall;
    int nextSceneID;
    bool throwPokeBall;
};

class Scene
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState getState() const { return mGameState; }
    void throwPokeBall() { mGameState.throwPokeBall = true; }
    void changeScene(int newId) { mGameState.nextSceneID = newId; }

    Vector2 getOrigin() const { return mOrigin; }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif