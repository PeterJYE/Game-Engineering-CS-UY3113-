/**
* Author: Peter Ye
* Assignment: Rise of the AI
* Date due: 2025-04-04, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/Scene.h"

constexpr int SCREEN_WIDTH  = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FPS           = 120;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr float GRAVITY        = 981.0f;
constexpr float TILE_SIZE      = 64.0f;

constexpr int MENU_SCENE_ID        = 0;
constexpr int LEVEL_ONE_SCENE_ID   = 1;
constexpr int LEVEL_TWO_SCENE_ID   = 2;
constexpr int LEVEL_THREE_SCENE_ID = 3;
constexpr int WIN_SCENE_ID         = 4;
constexpr int LOSE_SCENE_ID        = 5;

constexpr int LEVEL_WIDTH  = 28;
constexpr int LEVEL_HEIGHT = 10;

constexpr int GEORGE_ROWS      = 4;
constexpr int GEORGE_COLS      = 4;
constexpr int SOLDIER_ROWS     = 4;
constexpr int SOLDIER_COLS     = 4;
constexpr int GEORGE_START_ROW = 10;
constexpr int GEORGE_START_COL = 2;

constexpr int PLAYER_STARTING_LIVES = 3;
constexpr float TRAP_TRIGGER_DISTANCE = 70.0f;

enum GeorgeFacing { GEORGE_FACING_LEFT, GEORGE_FACING_RIGHT };

unsigned int LEVEL_ONE_DATA[] = {
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 6, 1, 0, 0, 0, 1, 1, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6,
    1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 6, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1
};

unsigned int LEVEL_TWO_DATA[] = {
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 6, 1, 0, 0, 0, 1, 1, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 5, 6,
    1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 6, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1
};

unsigned int LEVEL_THREE_DATA[] = {
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 6, 1, 0, 0, 0, 1, 1, 6,
    6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    6, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 9, 9, 0, 0, 0, 6,
    6, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 5, 6,
    1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 6, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1
};

constexpr char WINDOW_TITLE[]        = "Project 4";
constexpr char MENU_BACKGROUND_FP[]  = "assets/backgrou.png";
constexpr char MENU_PROMPT[]         = "Space to Start";
constexpr char MENU_RETURN_PROMPT[]  = "Press Space to Return to Menu";
constexpr char BGM_FP[]              = "assets/Dentaneosuchus Hunt.mp3";
constexpr char BULLET_SFX_FP[]       = "assets/bullet.wav";
constexpr char LAVA_HIT_SFX_FP[]     = "assets/lava_hit.mp3";
constexpr char UFO_HIT_SFX_FP[]      = "assets/ufo_hit.wav";
constexpr char PLAYER_TEXTURE_FP[]   = "assets/george.png";
constexpr char SOLDIER_TEXTURE_FP[]  = "assets/soldier.png";
constexpr char UFO_TEXTURE_FP[]      = "assets/skull_in_a_ufo_spacecraft.png";
constexpr char PLATFORM_TEXTURE_FP[] = "assets/grasstop.png";
constexpr char KEY_TEXTURE_FP[]      = "assets/key.png";
constexpr char DOOR_TEXTURE_FP[]     = "assets/door.png";
constexpr char WALL_TEXTURE_FP[]     = "assets/lava_spr_strip45.png";

AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f;
float gTimeAccumulator = 0.0f;
Camera2D gCamera       = { 0 };
int gLivesLeft         = PLAYER_STARTING_LIVES;
Music gBackgroundMusic = { 0 };
Sound gBulletSound     = { 0 };
Sound gLavaHitSound    = { 0 };
Sound gUfoHitSound     = { 0 };

void resetLives()
{
    gLivesLeft = PLAYER_STARTING_LIVES;
}

void initialiseBoundaryWalls(unsigned int *levelData)
{
    for (int row = 0; row < LEVEL_HEIGHT; row++)
    {
        levelData[row * LEVEL_WIDTH] = 6;
        levelData[row * LEVEL_WIDTH + (LEVEL_WIDTH - 1)] = 6;
    }
}

bool isTouching(Entity *entityA, Entity *entityB)
{
    float xDistance = fabs(entityA->getPosition().x - entityB->getPosition().x) -
                      ((entityA->getColliderDimensions().x + entityB->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(entityA->getPosition().y - entityB->getPosition().y) -
                      ((entityA->getColliderDimensions().y + entityB->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

bool isPointInsideEntity(Vector2 point, Entity *entity)
{
    Vector2 position = entity->getPosition();
    Vector2 collider = entity->getColliderDimensions();

    return point.x >= position.x - collider.x / 2.0f &&
           point.x <= position.x + collider.x / 2.0f &&
           point.y >= position.y - collider.y / 2.0f &&
           point.y <= position.y + collider.y / 2.0f;
}

class MenuScene : public Scene
{
private:
    Texture2D mBackgroundTexture = { 0 };

public:
    MenuScene(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

    void initialise() override
    {
        mGameState.hinako      = nullptr;
        mGameState.map         = nullptr;
        mGameState.nextSceneID = -1;
        mBackgroundTexture     = LoadTexture(MENU_BACKGROUND_FP);
    }

    void update(float deltaTime) override
    {
        (void) deltaTime;

        if (IsKeyPressed(KEY_SPACE))
        {
            resetLives();
            mGameState.nextSceneID = LEVEL_ONE_SCENE_ID;
        }
    }

    void render() override
    {
        ClearBackground(ColorFromHex(getBGColourHexCode()));

        Rectangle textureArea = {
            0.0f,
            0.0f,
            static_cast<float>(mBackgroundTexture.width),
            static_cast<float>(mBackgroundTexture.height)
        };

        Rectangle destinationArea = {
            0.0f,
            0.0f,
            static_cast<float>(SCREEN_WIDTH),
            static_cast<float>(SCREEN_HEIGHT)
        };

        DrawTexturePro(
            mBackgroundTexture,
            textureArea,
            destinationArea,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        int promptFontSize = 36;
        int promptWidth    = MeasureText(MENU_PROMPT, promptFontSize);

        DrawText(
            MENU_PROMPT,
            (SCREEN_WIDTH - promptWidth) / 2,
            SCREEN_HEIGHT - 70,
            promptFontSize,
            BLACK
        );
    }

    void shutdown() override
    {
        UnloadTexture(mBackgroundTexture);
    }
};

class WinScene : public Scene
{
public:
    WinScene(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

    void initialise() override
    {
        mGameState.hinako      = nullptr;
        mGameState.map         = nullptr;
        mGameState.nextSceneID = -1;
    }

    void update(float deltaTime) override
    {
        (void) deltaTime;

        if (IsKeyPressed(KEY_SPACE))
        {
            resetLives();
            mGameState.nextSceneID = MENU_SCENE_ID;
        }
    }

    void render() override
    {
        ClearBackground(ColorFromHex(getBGColourHexCode()));

        const char *title = "You Win";
        int titleFontSize = 56;
        int promptFontSize = 28;

        DrawText(
            title,
            (SCREEN_WIDTH - MeasureText(title, titleFontSize)) / 2,
            SCREEN_HEIGHT / 2 - 50,
            titleFontSize,
            BLACK
        );

        DrawText(
            MENU_RETURN_PROMPT,
            (SCREEN_WIDTH - MeasureText(MENU_RETURN_PROMPT, promptFontSize)) / 2,
            SCREEN_HEIGHT / 2 + 25,
            promptFontSize,
            BLACK
        );
    }

    void shutdown() override {}
};

class LoseScene : public Scene
{
public:
    LoseScene(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

    void initialise() override
    {
        mGameState.hinako      = nullptr;
        mGameState.map         = nullptr;
        mGameState.nextSceneID = -1;
    }

    void update(float deltaTime) override
    {
        (void) deltaTime;

        if (IsKeyPressed(KEY_SPACE))
        {
            resetLives();
            mGameState.nextSceneID = MENU_SCENE_ID;
        }
    }

    void render() override
    {
        ClearBackground(ColorFromHex(getBGColourHexCode()));

        const char *title = "You Lose";
        int titleFontSize = 56;
        int promptFontSize = 28;

        DrawText(
            title,
            (SCREEN_WIDTH - MeasureText(title, titleFontSize)) / 2,
            SCREEN_HEIGHT / 2 - 50,
            titleFontSize,
            WHITE
        );

        DrawText(
            MENU_RETURN_PROMPT,
            (SCREEN_WIDTH - MeasureText(MENU_RETURN_PROMPT, promptFontSize)) / 2,
            SCREEN_HEIGHT / 2 + 25,
            promptFontSize,
            WHITE
        );
    }

    void shutdown() override {}
};

class LevelScene : public Scene
{
private:
    int mLevelID;
    int mCompletionSceneID;
    unsigned int *mLevelData;

    GeorgeFacing mGeorgeFacing = GEORGE_FACING_RIGHT;
    Entity *mKey = nullptr;
    Entity *mDoor = nullptr;
    Entity *mUfo = nullptr;
    std::vector<Entity*> mWalls = {};
    std::vector<Entity*> mSoldiers = {};
    std::vector<int> mSoldierDirections = {};

    bool mKeyCollected = false;
    bool mWasTouchingHazard = false;

public:
    LevelScene(int levelID, int completionSceneID, unsigned int *levelData,
               Vector2 origin, const char *bgHexCode)
        : Scene(origin, bgHexCode),
          mLevelID(levelID),
          mCompletionSceneID(completionSceneID),
          mLevelData(levelData) {}

    void initialise() override
    {
        initialiseBoundaryWalls(mLevelData);

        mGameState.nextSceneID = -1;
        mGameState.map = new Map(
            LEVEL_WIDTH,
            LEVEL_HEIGHT,
            mLevelData,
            PLATFORM_TEXTURE_FP,
            TILE_SIZE,
            1,
            1,
            getOrigin()
        );

        initialiseGeorge();
        initialiseKey();
        initialiseDoor();
        initialiseUfo();
        initialiseWalls();
        initialiseSoldiers();
    }

    void update(float deltaTime) override
    {
        triggerTrapTiles();

        mGameState.hinako->update(
            deltaTime,
            nullptr,
            mGameState.map,
            mWalls.empty() ? nullptr : mWalls.data(),
            static_cast<int>(mWalls.size())
        );

        if (mGameState.hinako->getPosition().y >
            mGameState.map->getBottomBoundary() + TILE_SIZE)
        {
            gLivesLeft = 0;
            mGameState.nextSceneID = LOSE_SCENE_ID;
            return;
        }

        if (mKey != nullptr && !mKeyCollected &&
            isTouching(mGameState.hinako, mKey))
        {
            mKeyCollected = true;
            mKey->setAIState(WALKING);
        }

        if (mKey != nullptr && mKeyCollected)
        {
            mKey->update(deltaTime, mGameState.hinako, nullptr, nullptr, 0);

            Vector2 keyPosition = mKey->getPosition();
            keyPosition.y = mGameState.hinako->getPosition().y - 40.0f;
            mKey->setPosition(keyPosition);
        }

        updateUfo(deltaTime);

        updateSoldiers(deltaTime);

        if (mDoor != nullptr && mKeyCollected &&
            isTouching(mGameState.hinako, mDoor))
        {
            mGameState.nextSceneID = mCompletionSceneID;
            return;
        }

        bool isTouchingLava = isPlayerTouchingDamagingWall();
        bool isTouchingSoldier = isPlayerTouchingSoldier();
        bool isTouchingUfo = isPlayerTouchingUfo();
        bool isTouchingHazard = isTouchingLava ||
                                isTouchingSoldier ||
                                isTouchingUfo;

        if (isTouchingHazard && !mWasTouchingHazard)
        {
            if (isTouchingLava) PlaySound(gLavaHitSound);
            if (isTouchingSoldier) PlaySound(gBulletSound);
            if (isTouchingUfo) PlaySound(gUfoHitSound);
            gLivesLeft--;

            if (gLivesLeft <= 0)
            {
                gLivesLeft = 0;
                mGameState.nextSceneID = LOSE_SCENE_ID;
                return;
            }
        }

        mWasTouchingHazard = isTouchingHazard;

        updateGeorgeAnimation();
    }

    void render() override
    {
        ClearBackground(ColorFromHex(getBGColourHexCode()));
        mGameState.map->render();

        for (Entity *wall : mWalls) wall->render();
        for (Entity *soldier : mSoldiers) soldier->render();

        if (mDoor != nullptr) mDoor->render();
        if (mKey != nullptr) mKey->render();
        if (mUfo != nullptr) mUfo->render();

        mGameState.hinako->render();
    }

    void shutdown() override
    {
        for (Entity *wall : mWalls) delete wall;
        for (Entity *soldier : mSoldiers) delete soldier;

        delete mDoor;
        delete mKey;
        delete mUfo;
        delete mGameState.hinako;
        delete mGameState.map;

        mWalls.clear();
        mSoldiers.clear();
        mSoldierDirections.clear();
        mDoor = nullptr;
        mKey = nullptr;
        mUfo = nullptr;
        mGameState.hinako = nullptr;
        mGameState.map = nullptr;
    }

    bool usesCamera() const override { return true; }
    int getLevelID() const { return mLevelID; }

private:
    void triggerTrapTiles()
    {
        for (int row = 0; row < LEVEL_HEIGHT; row++)
        {
            for (int col = 0; col < LEVEL_WIDTH; col++)
            {
                if (mLevelData[row * LEVEL_WIDTH + col] != 9) continue;

                Vector2 trapTilePosition = {
                    mGameState.map->getLeftBoundary() + TILE_SIZE * (col + 0.5f),
                    mGameState.map->getTopBoundary() + TILE_SIZE * (row + 0.5f)
                };

                if (Vector2Distance(mGameState.hinako->getPosition(), trapTilePosition) <=
                    TRAP_TRIGGER_DISTANCE)
                {
                    mLevelData[row * LEVEL_WIDTH + col] = 0;
                }
            }
        }
    }

    void initialiseGeorge()
    {
        std::map<Animation, std::string> georgeTextures = {
            { WALK_LEFT,  PLAYER_TEXTURE_FP },
            { WALK_RIGHT, PLAYER_TEXTURE_FP },
            { IDLE_LEFT,  PLAYER_TEXTURE_FP },
            { IDLE_RIGHT, PLAYER_TEXTURE_FP },
            { CHARGING,   PLAYER_TEXTURE_FP }
        };

        std::map<Animation, Vector2> georgeSpriteSheetDimensions = {
            { WALK_LEFT,  { GEORGE_ROWS, GEORGE_COLS } },
            { WALK_RIGHT, { GEORGE_ROWS, GEORGE_COLS } },
            { IDLE_LEFT,  { GEORGE_ROWS, GEORGE_COLS } },
            { IDLE_RIGHT, { GEORGE_ROWS, GEORGE_COLS } },
            { CHARGING,   { GEORGE_ROWS, GEORGE_COLS } }
        };

        std::map<Animation, std::vector<int>> georgeAnimationAtlas = {
            { IDLE_LEFT,  { 9 } },
            { IDLE_RIGHT, { 11 } },
            { WALK_LEFT,  { 1, 5, 9, 13 } },
            { CHARGING,   { 1, 5, 9, 13 } },
            { WALK_RIGHT, { 3, 7, 11, 15 } }
        };

        mGameState.hinako = new Entity(
            {
                mGameState.map->getLeftBoundary() + TILE_SIZE * (GEORGE_START_COL - 0.5f),
                mGameState.map->getTopBoundary() + TILE_SIZE * (GEORGE_START_ROW - 1.0f) - 30.0f
            },
            { 56.0f, 76.0f },
            georgeTextures,
            ATLAS,
            georgeSpriteSheetDimensions,
            georgeAnimationAtlas,
            PLAYER
        );

        mGameState.hinako->setAcceleration({ 0.0f, GRAVITY });
        mGameState.hinako->setJumpingPower(500.0f);
        mGameState.hinako->setSpeed(260);
        mGameState.hinako->setFrameSpeed(8);
        mGameState.hinako->setColliderDimensions({ 38.0f, 60.0f });
        mGameState.hinako->setRenderOffset({ 0.0f, 18.0f });
        mGameState.hinako->setAnimation(IDLE_RIGHT);

        mGeorgeFacing = GEORGE_FACING_RIGHT;
        mKeyCollected = false;
        mWasTouchingHazard = false;
    }

    void initialiseKey()
    {
        for (int row = 0; row < LEVEL_HEIGHT; row++)
        {
            for (int col = 0; col < LEVEL_WIDTH; col++)
            {
                if (mLevelData[row * LEVEL_WIDTH + col] != 4) continue;

                mKey = new Entity(
                    {
                        mGameState.map->getLeftBoundary() + TILE_SIZE * (col + 0.5f),
                        mGameState.map->getTopBoundary() + TILE_SIZE * (row + 0.5f)
                    },
                    { 28.0f, 28.0f },
                    KEY_TEXTURE_FP,
                    NPC
                );

                mKey->setColliderDimensions({ 20.0f, 20.0f });
                mKey->setSpeed(160);
                mKey->setAIType(FOLLOWER);
                mKey->setAIState(IDLE);
            }
        }
    }

    void initialiseDoor()
    {
        for (int row = 0; row < LEVEL_HEIGHT; row++)
        {
            for (int col = 0; col < LEVEL_WIDTH; col++)
            {
                if (mLevelData[row * LEVEL_WIDTH + col] != 5) continue;

                mDoor = new Entity(
                    {
                        mGameState.map->getLeftBoundary() + TILE_SIZE * (col + 0.5f),
                        mGameState.map->getTopBoundary() + TILE_SIZE * (row + 0.5f)
                    },
                    { 48.0f, 64.0f },
                    DOOR_TEXTURE_FP,
                    BLOCK
                );

                mDoor->setColliderDimensions({ 36.0f, 56.0f });
                mDoor->setRenderOffset({ 0.0f, 30.0f });
            }
        }
    }

    void initialiseUfo()
    {
        if (mLevelID != LEVEL_THREE_SCENE_ID) return;

        mUfo = new Entity(
            {
                mGameState.map->getRightBoundary() - TILE_SIZE * 1.2f,
                mGameState.map->getTopBoundary() + TILE_SIZE * 0.8f
            },
            { 72.0f, 72.0f },
            UFO_TEXTURE_FP,
            NPC
        );

        mUfo->setColliderDimensions({ 50.0f, 40.0f });
        mUfo->setSpeed(95);
        mUfo->setAcceleration({ 0.0f, 0.0f });
        mUfo->setAIType(FOLLOWER);
        mUfo->setAIState(WALKING);
    }

    void initialiseWalls()
    {
        for (int row = 0; row < LEVEL_HEIGHT; row++)
        {
            for (int col = 0; col < LEVEL_WIDTH; col++)
            {
                if (mLevelData[row * LEVEL_WIDTH + col] != 6) continue;

                Entity *wall = new Entity(
                    {
                        mGameState.map->getLeftBoundary() + TILE_SIZE * (col + 0.5f),
                        mGameState.map->getTopBoundary() + TILE_SIZE * (row + 0.5f)
                    },
                    { 52.0f, 64.0f },
                    WALL_TEXTURE_FP,
                    PLATFORM
                );

                wall->setColliderDimensions({ 40.0f, 56.0f });
                wall->setRenderOffset({ 0.0f, 10.0f });
                mWalls.push_back(wall);
            }
        }
    }

    void initialiseSoldiers()
    {
        std::map<Animation, std::string> soldierTextures = {
            { WALK_LEFT,  SOLDIER_TEXTURE_FP },
            { WALK_RIGHT, SOLDIER_TEXTURE_FP },
            { IDLE_LEFT,  SOLDIER_TEXTURE_FP },
            { IDLE_RIGHT, SOLDIER_TEXTURE_FP },
            { CHARGING,   SOLDIER_TEXTURE_FP }
        };

        std::map<Animation, Vector2> soldierSpriteSheetDimensions = {
            { WALK_LEFT,  { SOLDIER_ROWS, SOLDIER_COLS } },
            { WALK_RIGHT, { SOLDIER_ROWS, SOLDIER_COLS } },
            { IDLE_LEFT,  { SOLDIER_ROWS, SOLDIER_COLS } },
            { IDLE_RIGHT, { SOLDIER_ROWS, SOLDIER_COLS } },
            { CHARGING,   { SOLDIER_ROWS, SOLDIER_COLS } }
        };

        std::map<Animation, std::vector<int>> soldierAnimationAtlas = {
            { IDLE_LEFT,  { 12, 13, 14, 15 } },
            { IDLE_RIGHT, { 8, 9, 10, 11 } },
            { WALK_LEFT,  { 12, 13, 14, 15 } },
            { WALK_RIGHT, { 8, 9, 10, 11 } },
            { CHARGING,   { 8, 9, 10, 11 } }
        };

        for (int row = 0; row < LEVEL_HEIGHT; row++)
        {
            for (int col = 0; col < LEVEL_WIDTH; col++)
            {
                if (mLevelData[row * LEVEL_WIDTH + col] != 8) continue;

                Entity *soldier = new Entity(
                    {
                        mGameState.map->getLeftBoundary() + TILE_SIZE * (col + 0.5f),
                        mGameState.map->getTopBoundary() + TILE_SIZE * (row + 0.5f) + 10.0f
                    },
                    { 56.0f, 72.0f },
                    soldierTextures,
                    ATLAS,
                    soldierSpriteSheetDimensions,
                    soldierAnimationAtlas,
                    PLATFORM
                );

                int direction = (static_cast<int>(mSoldiers.size()) % 2 == 0) ? -1 : 1;
                soldier->setAcceleration({ 0.0f, GRAVITY });
                soldier->setSpeed(110);
                soldier->setFrameSpeed(8);
                soldier->setColliderDimensions({ 34.0f, 58.0f });
                soldier->setRenderOffset({ 0.0f, 20.0f });
                soldier->setAnimation(direction < 0 ? WALK_LEFT : WALK_RIGHT);
                soldier->setMovement({ static_cast<float>(direction), 0.0f });

                mSoldiers.push_back(soldier);
                mSoldierDirections.push_back(direction);
            }
        }
    }

    void updateSoldiers(float deltaTime)
    {
        for (size_t i = 0; i < mSoldiers.size(); i++)
        {
            Entity *soldier = mSoldiers[i];

            updatePatrolSoldier(soldier, mSoldierDirections[i]);

            soldier->update(
                deltaTime,
                nullptr,
                mGameState.map,
                mWalls.empty() ? nullptr : mWalls.data(),
                static_cast<int>(mWalls.size())
            );
        }
    }

    void updateUfo(float deltaTime)
    {
        if (mUfo == nullptr) return;

        mUfo->update(deltaTime, mGameState.hinako, nullptr, nullptr, 0);

        Vector2 ufoPosition = mUfo->getPosition();
        ufoPosition.y = mGameState.hinako->getPosition().y;
        mUfo->setPosition(ufoPosition);
    }

    void updatePatrolSoldier(Entity *soldier, int &direction)
    {
        if (shouldSoldierTurnAround(soldier, direction))
            direction *= -1;

        soldier->setMovement({ static_cast<float>(direction), 0.0f });
        soldier->setAnimation(direction < 0 ? WALK_LEFT : WALK_RIGHT);
    }

    bool shouldSoldierTurnAround(Entity *soldier, int direction)
    {
        float halfWidth = soldier->getColliderDimensions().x / 2.0f;
        float halfHeight = soldier->getColliderDimensions().y / 2.0f;

        Vector2 sideProbe = {
            soldier->getPosition().x + direction * (halfWidth + 6.0f),
            soldier->getPosition().y
        };

        Vector2 groundProbe = {
            soldier->getPosition().x + direction * (halfWidth * 0.6f),
            soldier->getPosition().y + halfHeight + 6.0f
        };

        return isSolidAt(sideProbe) || !isSolidAt(groundProbe);
    }

    bool isSolidAt(Vector2 point)
    {
        float xOverlap = 0.0f;
        float yOverlap = 0.0f;

        if (mGameState.map->isSolidTileAt(point, &xOverlap, &yOverlap))
            return true;

        for (Entity *wall : mWalls)
        {
            if (isPointInsideEntity(point, wall))
                return true;
        }

        return false;
    }

    bool isPlayerTouchingDamagingWall()
    {
        for (Entity *wall : mWalls)
        {
            if (isTouchingDamagingWall(mGameState.hinako, wall))
                return true;
        }

        return false;
    }

    bool isPlayerTouchingSoldier()
    {
        for (Entity *soldier : mSoldiers)
        {
            if (isTouching(mGameState.hinako, soldier))
                return true;
        }

        return false;
    }

    bool isPlayerTouchingUfo()
    {
        return mUfo != nullptr && isTouching(mGameState.hinako, mUfo);
    }

    bool isTouchingDamagingWall(Entity *player, Entity *wall)
    {
        constexpr float SIDE_CONTACT_BUFFER = 8.0f;
        constexpr float TOP_CONTACT_BUFFER  = 4.0f;

        float xDistance = fabs(player->getPosition().x - wall->getPosition().x) -
                          ((player->getColliderDimensions().x + wall->getColliderDimensions().x) / 2.0f);
        float yDistance = fabs(player->getPosition().y - wall->getPosition().y) -
                          ((player->getColliderDimensions().y + wall->getColliderDimensions().y) / 2.0f);

        bool isStandingOnTop = player->getPosition().y < wall->getPosition().y &&
                               yDistance >= -TOP_CONTACT_BUFFER &&
                               yDistance <= TOP_CONTACT_BUFFER &&
                               xDistance < -TOP_CONTACT_BUFFER;

        bool isHittingSideOrBottom = xDistance <= SIDE_CONTACT_BUFFER &&
                                     yDistance <= SIDE_CONTACT_BUFFER;

        return isHittingSideOrBottom && !isStandingOnTop;
    }

    void updateGeorgeAnimation()
    {
        if (mGameState.hinako->getMovement().x < 0.0f)
            mGeorgeFacing = GEORGE_FACING_LEFT;
        else if (mGameState.hinako->getMovement().x > 0.0f)
            mGeorgeFacing = GEORGE_FACING_RIGHT;

        if (mGameState.hinako->getMovement().x < 0.0f)
            mGameState.hinako->setAnimation(WALK_LEFT);
        else if (mGameState.hinako->getMovement().x > 0.0f)
            mGameState.hinako->setAnimation(WALK_RIGHT);
        else if (!mGameState.hinako->isCollidingBottom() &&
                 mGeorgeFacing == GEORGE_FACING_LEFT)
            mGameState.hinako->setAnimation(WALK_LEFT);
        else if (!mGameState.hinako->isCollidingBottom())
            mGameState.hinako->setAnimation(WALK_RIGHT);
        else if (mGeorgeFacing == GEORGE_FACING_LEFT)
            mGameState.hinako->setAnimation(IDLE_LEFT);
        else
            mGameState.hinako->setAnimation(IDLE_RIGHT);
    }
};

Scene *gCurrentScene = nullptr;
MenuScene *gMenuScene = nullptr;
LevelScene *gLevelOneScene = nullptr;
LevelScene *gLevelTwoScene = nullptr;
LevelScene *gLevelThreeScene = nullptr;
WinScene *gWinScene = nullptr;
LoseScene *gLoseScene = nullptr;

void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

bool isGameplayScene(Scene *scene)
{
    return scene == gLevelOneScene ||
           scene == gLevelTwoScene ||
           scene == gLevelThreeScene;
}

void switchToDebugLevel(int levelSceneID)
{
    resetLives();

    if (levelSceneID == LEVEL_ONE_SCENE_ID) switchToScene(gLevelOneScene);
    if (levelSceneID == LEVEL_TWO_SCENE_ID) switchToScene(gLevelTwoScene);
    if (levelSceneID == LEVEL_THREE_SCENE_ID) switchToScene(gLevelThreeScene);
}

void switchToScene(Scene *scene)
{
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();

    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (gCurrentScene->usesCamera())
    {
        Vector2 currentPlayerPosition = {
            gCurrentScene->getState().hinako->getPosition().x,
            SCREEN_HEIGHT / 2.0f
        };
        gCamera.target = currentPlayerPosition;
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    InitAudioDevice();

    gBackgroundMusic = LoadMusicStream(BGM_FP);
    gBulletSound = LoadSound(BULLET_SFX_FP);
    gLavaHitSound = LoadSound(LAVA_HIT_SFX_FP);
    gUfoHitSound = LoadSound(UFO_HIT_SFX_FP);
    SetMusicVolume(gBackgroundMusic, 0.5f);
    PlayMusicStream(gBackgroundMusic);

    gMenuScene = new MenuScene(
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "#000000"
    );

    gLevelOneScene = new LevelScene(
        LEVEL_ONE_SCENE_ID,
        LEVEL_TWO_SCENE_ID,
        LEVEL_ONE_DATA,
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "#87CEEB"
    );

    gLevelTwoScene = new LevelScene(
        LEVEL_TWO_SCENE_ID,
        LEVEL_THREE_SCENE_ID,
        LEVEL_TWO_DATA,
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "#87CEEB"
    );

    gLevelThreeScene = new LevelScene(
        LEVEL_THREE_SCENE_ID,
        WIN_SCENE_ID,
        LEVEL_THREE_DATA,
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "#87CEEB"
    );

    gWinScene = new WinScene(
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "#90EE90"
    );

    gLoseScene = new LoseScene(
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        "#B22222"
    );

    gCamera.offset   = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    switchToScene(gMenuScene);
    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose())
    {
        gAppStatus = TERMINATED;
        return;
    }

    if (IsKeyPressed(KEY_ONE))
    {
        switchToDebugLevel(LEVEL_ONE_SCENE_ID);
        return;
    }

    if (IsKeyPressed(KEY_TWO))
    {
        switchToDebugLevel(LEVEL_TWO_SCENE_ID);
        return;
    }

    if (IsKeyPressed(KEY_THREE))
    {
        switchToDebugLevel(LEVEL_THREE_SCENE_ID);
        return;
    }

    if (isGameplayScene(gCurrentScene))
    {
        gCurrentScene->getState().hinako->resetMovement();

        if      (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  gCurrentScene->getState().hinako->moveLeft();
        else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) gCurrentScene->getState().hinako->moveRight();

        if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) &&
            gCurrentScene->getState().hinako->isCollidingBottom())
        {
            gCurrentScene->getState().hinako->jump();
        }

        if (GetLength(gCurrentScene->getState().hinako->getMovement()) > 1.0f)
            gCurrentScene->getState().hinako->normaliseMovement();
    }
}

void update()
{
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        UpdateMusicStream(gBackgroundMusic);
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        if (isGameplayScene(gCurrentScene))
        {
            Vector2 currentPlayerPosition = {
                gCurrentScene->getState().hinako->getPosition().x,
                SCREEN_HEIGHT / 2.0f
            };
            panCamera(&gCamera, &currentPlayerPosition);
        }
    }

    gTimeAccumulator = deltaTime;

    switch (gCurrentScene->getState().nextSceneID)
    {
        case MENU_SCENE_ID:
            if (gCurrentScene != gMenuScene) switchToScene(gMenuScene);
            break;
        case LEVEL_ONE_SCENE_ID:
            if (gCurrentScene != gLevelOneScene) switchToScene(gLevelOneScene);
            break;
        case LEVEL_TWO_SCENE_ID:
            if (gCurrentScene != gLevelTwoScene) switchToScene(gLevelTwoScene);
            break;
        case LEVEL_THREE_SCENE_ID:
            if (gCurrentScene != gLevelThreeScene) switchToScene(gLevelThreeScene);
            break;
        case WIN_SCENE_ID:
            if (gCurrentScene != gWinScene) switchToScene(gWinScene);
            break;
        case LOSE_SCENE_ID:
            if (gCurrentScene != gLoseScene) switchToScene(gLoseScene);
            break;
        default:
            break;
    }
}

void render()
{
    BeginDrawing();

    if (gCurrentScene->usesCamera()) BeginMode2D(gCamera);
    gCurrentScene->render();
    if (gCurrentScene->usesCamera()) EndMode2D();

    if (isGameplayScene(gCurrentScene))
    {
        char livesText[48];
        snprintf(livesText, sizeof(livesText), "Lives Left: %d", gLivesLeft);
        DrawText(livesText, 20, 20, 28, BLACK);
    }

    EndDrawing();
}

void shutdown()
{
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();

    delete gMenuScene;
    delete gLevelOneScene;
    delete gLevelTwoScene;
    delete gLevelThreeScene;
    delete gWinScene;
    delete gLoseScene;

    UnloadSound(gUfoHitSound);
    UnloadSound(gBulletSound);
    UnloadSound(gLavaHitSound);
    UnloadMusicStream(gBackgroundMusic);
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
        render();
    }

    shutdown();
    return 0;
}
