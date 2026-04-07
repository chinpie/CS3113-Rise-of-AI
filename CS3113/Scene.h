#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Entity *pokeTrainer;
    std::vector<Entity *> pokeBalls;
    Entity *enemyPokemon = nullptr;
    Entity *playerLives;
    int playerLifeCount;
    Map *map;
    Entity *diglett;
    Entity *pidgeotto;
    Entity *samurott;
    Music bgm;
    Sound jumpSound;
    Sound pokeCatch;
    Entity *BG1;
    Entity *pokeBall;
    int nextSceneID;
    bool throwPokeBall;
    Entity *heart;
    Sound pokeBallShake;
    int numPokeBalls;
    Entity *arceus;
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
    void setLife(int newLife) { mGameState.playerLifeCount = newLife; }
    void throwPokeBall() { mGameState.throwPokeBall = true; }
    void changeScene(int newId) { mGameState.nextSceneID = newId; }

    Vector2 getOrigin() const { return mOrigin; }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif