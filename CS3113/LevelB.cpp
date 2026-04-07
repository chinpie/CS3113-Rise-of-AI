#include "LevelB.h"

LevelB::LevelB() : Scene{{0.0f}, nullptr} {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene{origin, bgHexCode} {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
    mGameState.numPokeBalls = 20;
    mGameState.nextSceneID = 0;
    mGameState.enemyPokemon = new Entity[ENEMY_COUNT_2];
    mGameState.pokeBalls.clear();
    mGameState.throwPokeBall = false;

    mGameState.bgm = LoadMusicStream("assets/game/jam.mp3");
    SetMusicVolume(mGameState.bgm, 1.0f);
    PlayMusicStream(mGameState.bgm);

    mGameState.jumpSound = LoadSound("assets/game/jump.mp3");
    mGameState.pokeBallShake = LoadSound("assets/game/pokeballshake.mp3");
    mGameState.pokeCatch = LoadSound("assets/game/pokeBallCatch.mp3");

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
    std::map<Direction, std::vector<int>> pidgeottoAnimationAtlas = {
        {IDLING, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16}},
        {LEFT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}},
        {UP, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}},
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}},
    };

    std::map<Direction, std::vector<int>> samurottAnimationAtlas = {
        {IDLING, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {LEFT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {UP, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
        {RIGHT, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}},
    };
    std::map<Direction, std::vector<int>> pokeBallAnimationAtlas = {
        {SPINNING, {0, 1}},
        {IDLING, {0}},
        {CAUGHT, {0}},
        {RIGHT, {0}}};

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
    mGameState.pidgeotto = new Entity(
        {mOrigin.x - 350.0f, mOrigin.y - 800.0f}, // position
        {75.0f, 75.0f},                           // scale
        "assets/game/pidgeotto.png",              // texture file address
        ATLAS,
        {8, 18},                 // single image or atlas?                            // atlas dimensions
        pidgeottoAnimationAtlas, // actual atlas
        NPC                      // entity type
    );
    mGameState.BG1 = new Entity(
        {mOrigin.x - 300, mOrigin.y}, // position
        {2000, 1000},                 // scale
        "assets/game/cavebg.png",     // texture file address
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
    mGameState.playerLives = new Entity[mGameState.pokeTrainer->getPlayerLives()];
    initialLives = mGameState.pokeTrainer->getPlayerLives();
    playerHit = LoadSound("assets/game/minecraftdamage.mp3");

    mGameState.pokeTrainer->setJumpingPower(600.0f);
    mGameState.pokeTrainer->setColliderDimensions({mGameState.pokeTrainer->getScale().x / 2.0f,
                                                   mGameState.pokeTrainer->getScale().y});
    mGameState.pidgeotto->setColliderDimensions({mGameState.pidgeotto->getScale().x / 2, mGameState.pidgeotto->getScale().y / 2});
    mGameState.pidgeotto->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    mGameState.pokeTrainer->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY - 300.0f});
    mGameState.pokeBall->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY - 300.0f});
    mGameState.pokeBall->setColliderDimensions({mGameState.pokeBall->getScale().x / 2.0f, mGameState.pokeBall->getScale().y / 2.0f});
    mGameState.pidgeotto->setSpeed(20);
    mGameState.pokeTrainer->setPlayerLives(mGameState.playerLifeCount);
    for (int i = 0; i < ENEMY_COUNT_2; ++i)
    {
        mGameState.enemyPokemon[i] = *mGameState.pidgeotto;
        mGameState.enemyPokemon[i].setAIType(LERPER);
        mGameState.enemyPokemon[i].setAIState(IDLE);
        mGameState.enemyPokemon[i].setPosition({mOrigin.x + (i * 100.0f), mOrigin.y});
    }
    for (int i = 0; i < mGameState.pokeTrainer->getPlayerLives(); ++i)
    {
        mGameState.playerLives[i].setTexture("assets/game/heart.png");
        mGameState.playerLives[i].setScale({32.0f, 32.0f});
        mGameState.playerLives[i].setPosition({mGameState.pokeTrainer->getPosition().x - 50.0f + (i * 50.0f),
                                               mGameState.pokeTrainer->getPosition().y - 100.0f});
    }
    /**/
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    stageWon = true;

    mGameState.pokeTrainer->update(
        deltaTime,               // delta time / fixed timestep~
        nullptr,                 // player
        mGameState.map,          // map
        mGameState.enemyPokemon, // collidable entities
        ENEMY_COUNT_2            // col. entity count
    );
    if (initialLives != mGameState.pokeTrainer->getPlayerLives())
    {
        PlaySound(playerHit);
        initialLives = mGameState.pokeTrainer->getPlayerLives();
    }
    if (mGameState.throwPokeBall && mGameState.numPokeBalls > 0)
    {
        Entity *gNewPokeBall = new Entity(*mGameState.pokeBall);
        gNewPokeBall->setPosition({mGameState.pokeTrainer->getPosition().x,
                                   mGameState.pokeTrainer->getPosition().y});
        gNewPokeBall->setSpeed(100);
        gNewPokeBall->setVelocity({100.0f, -400.0f});
        gNewPokeBall->setTimer(GetTime());
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
        mGameState.numPokeBalls -= 1;
        mGameState.throwPokeBall = false;
    }
    for (size_t index = 0; index < mGameState.pokeBalls.size(); ++index)
    {
        mGameState.pokeBalls[index]->update(deltaTime, nullptr, mGameState.map, mGameState.enemyPokemon, ENEMY_COUNT_2);
        float pokeBallTime = GetTime() - mGameState.pokeBalls[index]->getTimer();
        if (mGameState.pokeBalls[index]->getDirection() == SPINNING && pokeBallTime >= 5.0f)
        {
            mGameState.pokeBalls[index]->setDirection(CAUGHT);
            PlaySound(mGameState.pokeCatch);
        }
        if (mGameState.pokeBalls[index]->getCanShake() == true)
        {
            PlaySound(mGameState.pokeBallShake);
            mGameState.pokeBalls[index]->setCanShake(false);
        }
    }
    for (int i = 0; i < ENEMY_COUNT_2; ++i)
    {
        mGameState.enemyPokemon[i].update(deltaTime, mGameState.pokeTrainer, mGameState.map, nullptr, 0);
        if (mGameState.enemyPokemon[i].checkActive() == ACTIVE)
        {
            stageWon = false;
        }
    }
    for (int i = 0; i < mGameState.pokeTrainer->getPlayerLives(); ++i)
    {
        mGameState.playerLives[i].setPosition({mGameState.pokeTrainer->getPosition().x - 50.0f + (i * 50.0f),
                                               mGameState.pokeTrainer->getPosition().y - 100.0f});
    }
    if (stageWon)
    {
        changeScene(3);
    }
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.BG1->render();
    mGameState.pokeTrainer->render();
    mGameState.map->render();
    for (size_t index = 0; index < mGameState.pokeBalls.size(); ++index)
    {
        mGameState.pokeBalls[index]->render();
    }
    for (int i = 0; i < ENEMY_COUNT_2; ++i)
    {
        mGameState.enemyPokemon[i].render();
    }
    for (int i = 0; i < mGameState.pokeTrainer->getPlayerLives(); ++i)
    {
        mGameState.playerLives[i].render();
    }
}

void LevelB::shutdown()
{
    delete mGameState.BG1;
    delete mGameState.pokeTrainer;
    delete mGameState.pidgeotto;
    delete mGameState.map;
    for (size_t index = 0; index < mGameState.pokeBalls.size(); ++index)
    {
        delete mGameState.pokeBalls[index];
    }
    if (mGameState.enemyPokemon != nullptr)
    {
        delete[] mGameState.enemyPokemon;
        mGameState.enemyPokemon = nullptr;
    }

    UnloadMusicStream(mGameState.bgm);
    UnloadSound(mGameState.jumpSound);
    UnloadSound(mGameState.pokeBallShake);
    UnloadSound(mGameState.pokeCatch);
}