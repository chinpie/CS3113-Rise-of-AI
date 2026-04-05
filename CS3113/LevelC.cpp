#include "LevelC.h"

LevelC::LevelC() : Scene{{0.0f}, nullptr} {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{
    mGameState.nextSceneID = 0;

    mGameState.bgm = LoadMusicStream("assets/game/jam.mp3");
    SetMusicVolume(mGameState.bgm, 1.0f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/jump.mp3");

    /*
       ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,  // map grid cols & rows
        (unsigned int *)mLevelData, // grid data
        "assets/game/tileset.png",  // texture filepath
        TILE_DIMENSION,             // tile size
        4, 1,                       // texture cols & rows
        mOrigin                     // in-game origin
    );

    /*
       ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> pokeTrainerAnimationAtlas = {
        {IDLING, {0, 1}},
        {LEFT, {5, 6, 7}},
        {UP, {18, 19, 20, 21, 22, 23, 24, 25}},
        {RIGHT, {2, 3, 4}},
    };
    std::map<Direction, std::vector<int>> diglettAnimationAtlas = {
        {IDLING, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {LEFT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {UP, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
    };

    std::map<Direction, std::vector<int>> samurottAnimationAtlas = {
        {IDLING, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {LEFT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {UP, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
    };
    std::map<Direction, std::vector<int>> pokeBallAnimationAtlas = {
        {IDLING, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {LEFT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {UP, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
    };

    float sizeRatio = 48.0f / 64.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    mGameState.pokeTrainer = new Entity(
        {mOrigin.x - 250.0f, mOrigin.y + 200}, // position
        {75.0f, 75.0f},                        // scale
        "assets/game/trainersprites.png",      // texture file address
        ATLAS,                                 // single image or atlas?
        {2, 5},                                // atlas dimensions
        pokeTrainerAnimationAtlas,             // actual atlas
        PLAYER                                 // entity type
    );
    mGameState.diglett = new Entity(
        {mOrigin.x - 350.0f, mOrigin.y + 100.0f}, // position
        {75.0f, 75.0f},                           // scale
        "assets/game/diglett_idle.png",           // texture file address
        ATLAS,
        {8, 2},                // single image or atlas?                            // atlas dimensions
        diglettAnimationAtlas, // actual atlas
        NPC                    // entity type
    );
    mGameState.BG1 = new Entity(
        {mOrigin.x - 300, mOrigin.y}, // position
        {2000, 1000},                 // scale
        "assets/game/bg1.png",        // texture file address
        SINGLE,
        {1, 1},                 // single image or atlas?                            // atlas dimensions
        samurottAnimationAtlas, // actual atlas
        BLOCK                   // entity type
    );
    mGameState.pokeBall = new Entity(
        {mOrigin.x - 300, mOrigin.y}, // position
        {50, 50},                     // scale
        "assets/game/pokeBall.png",   // texture file address
        ATLAS,
        {1, 2},                 // single image or atlas?                            // atlas dimensions
        pokeBallAnimationAtlas, // actual atlas
        POKEBALL                // entity type
    );

    mGameState.pokeTrainer->setJumpingPower(600.0f);
    mGameState.pokeTrainer->setColliderDimensions({mGameState.pokeTrainer->getScale().x,
                                                   mGameState.pokeTrainer->getScale().y});
    mGameState.diglett->setColliderDimensions({mGameState.diglett->getScale().x / 2, mGameState.diglett->getScale().y / 2});
    mGameState.diglett->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    mGameState.pokeTrainer->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY - 300});
    mGameState.pokeBall->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    mGameState.pokeBall->setColliderDimensions({mGameState.pokeBall->getScale().x * 2, mGameState.pokeBall->getScale().y * 2});
    mGameState.diglett->setSpeed(60);
    mGameState.diglett->setAIType(FOLLOWER);

    /**/
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);

    mGameState.pokeTrainer->update(
        deltaTime,          // delta time / fixed timestep~
        nullptr,            // player
        mGameState.map,     // map
        mGameState.diglett, // collidable entities
        1                   // col. entity count
    );
    mGameState.diglett->update(deltaTime,              // delta time / fixed timestep
                               mGameState.pokeTrainer, // player
                               mGameState.map,         // map
                               nullptr,                // collidable entities
                               0);
    mGameState.pokeBall->update(deltaTime, mGameState.pokeTrainer, mGameState.map, mGameState.diglett, 1);

    if (mGameState.pokeTrainer->getPosition().y > 800.0f)
        mGameState.nextSceneID = 2;
    for (size_t index = 0; index < mGameState.pokeBalls.size(); ++index)
    {
        mGameState.pokeBalls[index]->update(deltaTime, nullptr, mGameState.map, mGameState.diglett, 1);
    }
    if (mGameState.throwPokeBall)
    {
        Entity *gNewPokeBall = new Entity(*mGameState.pokeBall);
        gNewPokeBall->setPosition({mGameState.pokeTrainer->getPosition().x,
                                   mGameState.pokeTrainer->getPosition().y});
        gNewPokeBall->setSpeed(100);
        gNewPokeBall->setVelocity({100.0f, -400.0f});
        if (gNewPokeBall->getSpeed() > 0)
        {
            gNewPokeBall->setAcceleration({-5, ACCELERATION_OF_GRAVITY - 200});
        }
        else
        {
            gNewPokeBall->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY - 200});
            gNewPokeBall->setVelocity({0.0f, gNewPokeBall->getVelocity().y});
        }
        if (mGameState.pokeTrainer->getMovement().x == -1)
        {
            gNewPokeBall->setMovement({-1, -1});
        }
        else if (mGameState.pokeTrainer->getMovement().x == 1)
        {
            gNewPokeBall->setMovement({1, -1});
        }
        else
        {
            gNewPokeBall->setMovement({0, 1});
        }
        mGameState.pokeBalls.emplace_back(gNewPokeBall);
        mGameState.throwPokeBall = false;
    }
    if (mGameState.diglett->checkActive() == INACTIVE)
    {
        this->changeScene(2);
    }
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.BG1->render();
    mGameState.pokeTrainer->render();
    mGameState.diglett->render();
    mGameState.map->render();
    mGameState.pokeBall->render();
    for (size_t index = 0; index < mGameState.pokeBalls.size(); ++index)
    {
        mGameState.pokeBalls[index]->render();
    }
}

void LevelC::shutdown()
{
    delete mGameState.BG1;
    delete mGameState.pokeTrainer;
    delete mGameState.diglett;
    delete mGameState.map;

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
}