#include "CS3113/ShaderProgram.h"
#include "CS3113/Scene.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1040,
              SCREEN_HEIGHT = 720,
              FPS           = 120,
              TILE_SIZE     = 64,
              MAP_WIDTH     = 26,
              MAP_HEIGHT    = 18;

constexpr int SDLK_RETURN = KEY_ENTER;

constexpr char BG_COLOUR[] = "#1D2633";
constexpr char MENU_BG_COLOUR[] = "#111827";
constexpr char MENU_TEXT_COLOUR[] = "#F9FAFB";
constexpr char MENU_ACCENT_COLOUR[] = "#F2C14E";
constexpr char WINDOW_TITLE[] = "Animal Giant 2126";

constexpr char ROAD_FP[]      = "assets/Colored/tile_0017.png";
constexpr char WALL_FP[]      = "assets/Colored/tile_0148.png";
constexpr char ENDPOINT_FP[]  = "assets/Colored/tile_0117.png";
constexpr char CHARACTER_FP[] = "assets/Colored/george.png";
constexpr char FOOD_FP[]      = "assets/Colored/tile_0138.png";
constexpr char SNAKE_FP[]     = "assets/Colored/tile_0020.png";
constexpr char ZOMBIE_TILE_FP[] = "assets/Colored/tile_0011.png";
constexpr char DOG_FP[]       = "assets/Colored/Dog.png";
constexpr char DOG_BARK_FP[]  = "assets/Colored/dog_barking.wav";
constexpr char DOG_EATING_FP[] = "assets/Colored/dog_eating.mp3";
constexpr char ZOMBIE_FP[]    = "assets/Colored/zombie.mp3";
constexpr char ZOMBIE_BITE_FP[] = "assets/Colored/crunchybite.ogg";
constexpr char SNAKE_BITE_FP[] = "assets/Colored/crunchybite.ogg";
constexpr char BACKGROUND_MUSIC_FP[] = "assets/Colored/back.mp3";
constexpr char VERTEX_SHADER_FP[]   = "shader/vertex.glsl";
constexpr char FRAGMENT_SHADER_FP[] = "shader/fragment.glsl";

constexpr int MAP_OFFSET_X = 0,
              MAP_OFFSET_Y = 0,
              MAP_PIXEL_WIDTH = MAP_WIDTH * TILE_SIZE,
              MAP_PIXEL_HEIGHT = MAP_HEIGHT * TILE_SIZE,
              MENU_SCENE_ID = 0,
              LEVEL_ONE_SCENE_ID = 1,
              LEVEL_TWO_SCENE_ID = 2,
              LEVEL_THREE_SCENE_ID = 3,
              NUMBER_OF_LEVELS = 3,
              REQUIRED_FOOD = 3,
              STARTING_LIVES = 3,
              MAX_SNAKES = 8,
              MAX_ZOMBIES = 8,
              GEORGE_COLS = 4,
              GEORGE_ROWS = 4,
              GEORGE_DOWN = 0,
              GEORGE_LEFT = 1,
              GEORGE_UP = 2,
              GEORGE_RIGHT = 3,
              DOG_COLS = 8,
              DOG_ROWS = 2,
              DOG_FRAME_WIDTH = 16,
              DOG_FRAME_HEIGHT = 16,
              DOG_LEFT = 0,
              DOG_RIGHT = 1,
              DOG_WALK_FRAMES = 4;

constexpr float GEORGE_SCALE = 0.68f,
                BASE_MOVE_SPEED = 300.0f,
                FOOD_SLOWDOWN = 50.0f,
                LIFE_LOSS_SLOWDOWN = 40.0f,
                MIN_MOVE_SPEED = 100.0f,
                SNAKE_SCALE = 0.78f,
                SNAKE_SPEED = 140.0f,
                ZOMBIE_SCALE = 0.78f,
                ZOMBIE_SPEED = 50.0f,
                DOG_SCALE = 0.78f,
                DOG_SPEED = 140.0f,
                LIGHT_RADIUS = 170.0f,
                DAMAGE_COOLDOWN = 1.0f,
                FRAME_TIME   = 0.08f;

constexpr char WALL_TILE     = 'W',
               ROAD_TILE     = 'R',
               ENDPOINT_TILE = 'E',
               FOOD_TILE = 'C',
               SNAKE_TILE = 'S',
               DOG_TILE = 'D',
               ZOMBIE_TILE = 'Z';

struct Snake
{
    Vector2 position;
    float minX;
    float maxX;
    int direction;
};

struct Dog
{
    Vector2 position;
    Vector2 startPosition;
    bool isOnMap;
    bool isChasing;
    bool hasCaught;
    int direction;
    int frame;
    float frameTimer;
};

struct Zombie
{
    Vector2 position;
    float minY;
    float maxY;
    int direction;
};

const char LEVEL_MAPS[NUMBER_OF_LEVELS][MAP_HEIGHT][MAP_WIDTH + 1] = {
    {
        "WWWWWWWWWWWWWWWWWWWWWWWWWW",
        "WEWRRRRRRRRRRRRRRRRRWRRRRW",
        "WRWWWWRWWWWRWWWWRWWWWRRRRW",
        "WRRRRRRRRRWRRRRSRRRRRWRSRW",
        "WRWWRWWWWRWRWWWWWWWRRWRCRW",
        "WRWWRRRRRRRRRRRRRCRRRWRRRW",
        "WRWWWWWWWRWWWWWWWWWWRWRRRW",
        "WRRRRRRRWRRRRSRRRRRRRWRRRW",
        "WWWWWWRRWRWWWWWWRWWWWWRRRW",
        "WWRRRRRRWRRRRRRWRRRRRRRRRW",
        "WWRWWWWWWWWWWRRWRWWWWWWWRW",
        "WWRRRRRRRRRRRRRWRRRWRRRRRW",
        "WWWWWRWWWWWWWWWWWWRWRWWWRW",
        "WWRRRRWRRRRRRRRRRRRRRRRRRW",
        "WWRWWRWRWWWWRWWWWWWWWWRRRW",
        "WWCRRRSRRRRRWRRRRRRRRRRRRW",
        "WWWWWWWWWWWWRRRRRRRRRRRRRW",
        "WWWWWWWWWWWWWWWWWWWWWWWWWW"
    },
    {
        "WWWWWWWWWWWWWWWWWWWWWWWWWW",
        "WEWRCRRRRRRRRRRRRRRRWRRRRW",
        "WRRWWWRWWWWRWWWWRWWWWRRRRW",
        "WRRRRRRRRRWRRRRSRRRRRWRSRW",
        "WRWWRWWWWRWRWWWWWWWRRWRRRW",
        "WRWWRRRRRRRRRRRRRCRRRWRRRW",
        "WRWWWWWWWRWWWWWWWWWWRWRRRW",
        "WRRRRRRRWRRRRSRRRRRRRWRRRW",
        "WWWWWWRRWRWWWWWWRWWWWWRRRW",
        "WWRRRRRRWRRRRRRWRRRRRRRRRW",
        "WWRWWWWWWWWWWRRWRWWWWWWWRW",
        "WWRRRRRRRRRRRRRWRRRWRRRRRW",
        "WWWWWRWWWWWWWWWWWWRWRWWWRW",
        "WWRRRRWRRRRRRRRRRRRRRWRRRW",
        "WWRWWRWRWWWWRWWWWWWWWWWRRW",
        "WWCRRRSRRRRRWRRRRRRRRSRRRW",
        "WWWWWWWWWWWWDRRRRRRRRRCRRW",
        "WWWWWWWWWWWWWWWWWWWWWWWWWW"
    },
    {
        "WWWWWWWWWWWWWWWWWWWWWWWWWW",
        "WEWWWRRRRRRRRRRWRRRRRRRRRW",
        "WRRWWWRWWWWRWWWWRWWWWRRRRW",
        "WRRWRRRRRRWRRRRSRRRRRWRSRW",
        "WRWWRWWWWRWRWWWWWWWRRWRRRW",
        "WRWWRRRRRRRRRRRRRCRRRWRRRW",
        "WZWWWWWWWRWWWWWWWWWWRWRRRW",
        "WRRRRRRRWRRRRSRRRRRRRWRRWW",
        "WWWWWWRZWRWWWWWWRWWWWWRRRW",
        "WWRRRRRRWRRRRRRWRRRRRRRRRW",
        "WWRWWWWWWWWWWRRWRWWWWWWWRW",
        "WWRRRRRRRRRRRRRWRRRWRRRRRW",
        "WWWWWRWWWWWWWWWWWWRWRWWWRW",
        "WWRRRRWRRRRRRRRRRRRRRRRRRW",
        "WWSWWRWRWWWWRWWWWWWWWWRRRW",
        "WWCRRRRRRRRRWRRRRRRRSRRRZW",
        "WWWWWWWWWWWWRRRRRRRRRRCRRW",
        "WWWWWWWWWWWWWWWWWWWWWWWWWW"
    }
};

char gLevelData[MAP_HEIGHT][MAP_WIDTH + 1];

// Global Variables
AppStatus gAppStatus = RUNNING;

Texture2D gRoadTexture;
Texture2D gWallTexture;
Texture2D gEndpointTexture;
Texture2D gCharacterTexture;
Texture2D gFoodTexture;
Texture2D gSnakeTexture;
Texture2D gZombieTexture;
Texture2D gDogTexture;
Sound     gDogBarkSound;
Sound     gDogEatingSound;
Sound     gZombieSound;
Sound     gZombieBiteSound;
Sound     gSnakeBiteSound;
Music     gBackgroundMusic;
Camera2D gCamera = { 0 };
ShaderProgram gShader;

Vector2 gPlayerTile = { 1.0f, 1.0f },
        gPlayerPosition = {
            static_cast<float>(MAP_OFFSET_X + TILE_SIZE + TILE_SIZE / 2),
            static_cast<float>(MAP_OFFSET_Y + TILE_SIZE + TILE_SIZE / 2)
        },
        gPlayerTargetPosition = {
            static_cast<float>(MAP_OFFSET_X + TILE_SIZE + TILE_SIZE / 2),
            static_cast<float>(MAP_OFFSET_Y + TILE_SIZE + TILE_SIZE / 2)
        };
Vector2 gEndpointTile = { 1.0f, 1.0f };

float   gAngle         = 0.0f,
        gPreviousTicks = 0.0f,
        gFrameTimer    = 0.0f;
bool    gHasWon     = false;
bool    gHasLost    = false;
bool    gIsMoving   = false;
int     gCurrentLevel = 0;
int     gFoodCollected = 0;
int     gLives      = STARTING_LIVES;
int     gPlayerDirection = GEORGE_DOWN;
int     gPlayerFrame     = 0;
float   gDamageTimer = 0.0f;
Snake   gSnakes[MAX_SNAKES];
int     gSnakeCount = 0;
Zombie  gZombies[MAX_ZOMBIES];
int     gZombieCount = 0;
Dog     gDog = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, false, false, false, DOG_RIGHT, 0, 0.0f };
bool    gDogBarkPlayed = false;
float   gTimeAccumulator = 0.0f;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
bool    gShouldResetGame = true;

class MenuScene : public Scene
{
public:
    MenuScene(Vector2 origin, const char *bgHexCode) : Scene(origin, bgHexCode) {}

    void initialise() override
    {
        mGameState.nextSceneID = -1;
    }

    void update(float deltaTime) override
    {
        (void) deltaTime;

        if (IsKeyPressed((KeyboardKey) SDLK_RETURN))
            mGameState.nextSceneID = LEVEL_ONE_SCENE_ID;
    }

    void render() override
    {
        ClearBackground(ColorFromHex(getBGColourHexCode()));

        const char *title = WINDOW_TITLE;
        const char *prompt = "Press Enter to Start";
        const char *storyLineOne = "It is 2126 now.";
        const char *storyLineTwo = "All the animal alive becomes much larger,";
        const char *storyLineThree = "find enough food for you family. Don't Die!!!";
        const char *instructionsTitle = "Instructions";
        const char *instructionOne = "W A S D : move through the maze";
        const char *instructionTwo = "Collect 3 food before using the exit";
        const char *instructionThree = "Avoid snakes and the giant dog";
        const char *instructionFour = "R : restart the game";
        const char *instructionFive = "Q : quit the game";

        Color textColour = ColorFromHex(MENU_TEXT_COLOUR);
        Color accentColour = ColorFromHex(MENU_ACCENT_COLOUR);

        int titleSize = 54;
        int bodySize = 26;
        int smallSize = 22;

        DrawText(
            title,
            (SCREEN_WIDTH - MeasureText(title, titleSize)) / 2,
            90,
            titleSize,
            accentColour
        );

        DrawText(
            storyLineOne,
            (SCREEN_WIDTH - MeasureText(storyLineOne, bodySize)) / 2,
            190,
            bodySize,
            textColour
        );

        DrawText(
            storyLineTwo,
            (SCREEN_WIDTH - MeasureText(storyLineTwo, bodySize)) / 2,
            230,
            bodySize,
            textColour
        );

        DrawText(
            storyLineThree,
            (SCREEN_WIDTH - MeasureText(storyLineThree, bodySize)) / 2,
            270,
            bodySize,
            textColour
        );

        DrawText(
            instructionsTitle,
            (SCREEN_WIDTH - MeasureText(instructionsTitle, bodySize)) / 2,
            365,
            bodySize,
            accentColour
        );

        DrawText(instructionOne,   (SCREEN_WIDTH - MeasureText(instructionOne, smallSize)) / 2, 415, smallSize, textColour);
        DrawText(instructionTwo,   (SCREEN_WIDTH - MeasureText(instructionTwo, smallSize)) / 2, 450, smallSize, textColour);
        DrawText(instructionThree, (SCREEN_WIDTH - MeasureText(instructionThree, smallSize)) / 2, 485, smallSize, textColour);
        DrawText(instructionFour,  (SCREEN_WIDTH - MeasureText(instructionFour, smallSize)) / 2, 520, smallSize, textColour);
        DrawText(instructionFive,  (SCREEN_WIDTH - MeasureText(instructionFive, smallSize)) / 2, 555, smallSize, textColour);

        DrawText(
            prompt,
            (SCREEN_WIDTH - MeasureText(prompt, bodySize)) / 2,
            635,
            bodySize,
            accentColour
        );
    }

    void shutdown() override {}
};

class LevelScene : public Scene
{
private:
    int mLevelIndex;

public:
    LevelScene(int levelIndex, Vector2 origin, const char *bgHexCode)
        : Scene(origin, bgHexCode), mLevelIndex(levelIndex) {}

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

Scene *gCurrentScene = nullptr;
MenuScene *gMenuScene = nullptr;
LevelScene *gLevelOneScene = nullptr;
LevelScene *gLevelTwoScene = nullptr;
LevelScene *gLevelThreeScene = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialiseApp();
void processInput();
void updateApp();
void renderApp();
void shutdownApp();
void initialiseGameAssets();
void shutdownGameAssets();
void processGameplayInput();
void updateGameplay(float deltaTime);
void renderGameplay();
void resetGameState();
void drawTextureAtTile(const Texture2D *texture, int col, int row, float scale);
void drawGeorgeAtPosition(const Vector2 *position, float scale);
void drawSnakeAtPosition(const Snake *snake);
void drawZombieAtPosition(const Zombie *zombie);
void drawDogAtPosition(const Dog *dog);
void tryMovePlayer(int xDirection, int yDirection);
bool isWalkableTile(int col, int row);
bool isEndpointLocation(int col, int row);
float getPlayerSpeed();
void updateCamera();
void initialiseSnakes();
void updateSnakes(float deltaTime);
void checkSnakeCollisions(float deltaTime);
void initialiseZombies();
void updateZombies(float deltaTime);
void checkZombieCollisions(float deltaTime);
void initialiseDog();
void updateDog(float deltaTime);
void checkDogCollision();
bool isDogRoadPosition(const Vector2 *position);
bool isColliding(const Vector2 *positionA, float scaleA, const Vector2 *positionB, float scaleB);
void loadLevel(int levelIndex);

void drawTextureAtTile(const Texture2D *texture, int col, int row, float scale)
{
    Rectangle textureArea = {
        0.0f,
        0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        static_cast<float>(MAP_OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2),
        static_cast<float>(MAP_OFFSET_Y + row * TILE_SIZE + TILE_SIZE / 2),
        TILE_SIZE * scale,
        TILE_SIZE * scale
    };

    Vector2 originOffset = {
        destinationArea.width / 2.0f,
        destinationArea.height / 2.0f
    };

    DrawTexturePro(
        *texture,
        textureArea,
        destinationArea,
        originOffset,
        gAngle,
        WHITE
    );
}

void drawGeorgeAtPosition(const Vector2 *position, float scale)
{
    float frameWidth  = gCharacterTexture.width  / (float) GEORGE_COLS;
    float frameHeight = gCharacterTexture.height / (float) GEORGE_ROWS;

    Rectangle textureArea = {
        frameWidth * gPlayerDirection,
        frameHeight * gPlayerFrame,
        frameWidth,
        frameHeight
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        TILE_SIZE * scale,
        TILE_SIZE * scale
    };

    Vector2 originOffset = {
        destinationArea.width / 2.0f,
        destinationArea.height / 2.0f
    };

    DrawTexturePro(
        gCharacterTexture,
        textureArea,
        destinationArea,
        originOffset,
        gAngle,
        WHITE
    );
}

void drawSnakeAtPosition(const Snake *snake)
{
    Rectangle textureArea = {
        snake->direction < 0 ? 0.0f : static_cast<float>(gSnakeTexture.width),
        0.0f,
        snake->direction < 0
            ? static_cast<float>(gSnakeTexture.width)
            : static_cast<float>(-gSnakeTexture.width),
        static_cast<float>(gSnakeTexture.height)
    };

    Rectangle destinationArea = {
        snake->position.x,
        snake->position.y,
        TILE_SIZE * SNAKE_SCALE,
        TILE_SIZE * SNAKE_SCALE
    };

    Vector2 originOffset = {
        destinationArea.width / 2.0f,
        destinationArea.height / 2.0f
    };

    DrawTexturePro(
        gSnakeTexture,
        textureArea,
        destinationArea,
        originOffset,
        gAngle,
        WHITE
    );
}

void drawZombieAtPosition(const Zombie *zombie)
{
    Rectangle destinationArea = {
        zombie->position.x,
        zombie->position.y,
        TILE_SIZE * ZOMBIE_SCALE,
        TILE_SIZE * ZOMBIE_SCALE
    };

    Vector2 originOffset = {
        destinationArea.width / 2.0f,
        destinationArea.height / 2.0f
    };

    DrawTexturePro(
        gZombieTexture,
        { 0.0f, 0.0f, static_cast<float>(gZombieTexture.width), static_cast<float>(gZombieTexture.height) },
        destinationArea,
        originOffset,
        gAngle,
        WHITE
    );
}

void drawDogAtPosition(const Dog *dog)
{
    int baseFrame = dog->direction == DOG_RIGHT ? DOG_WALK_FRAMES : 0;
    int frameIndex = baseFrame + dog->frame;
    Rectangle textureArea = {
        static_cast<float>(frameIndex * DOG_FRAME_WIDTH),
        0.0f,
        static_cast<float>(DOG_FRAME_WIDTH),
        static_cast<float>(DOG_FRAME_HEIGHT)
    };

    Rectangle destinationArea = {
        dog->position.x,
        dog->position.y,
        TILE_SIZE * DOG_SCALE,
        TILE_SIZE * DOG_SCALE
    };

    Vector2 originOffset = {
        destinationArea.width / 2.0f,
        destinationArea.height / 2.0f
    };

    DrawTexturePro(
        gDogTexture,
        textureArea,
        destinationArea,
        originOffset,
        gAngle,
        WHITE
    );
}

bool isWalkableTile(int col, int row)
{
    if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT) return false;

    char tile = gLevelData[row][col];

    if (isEndpointLocation(col, row) && gFoodCollected < REQUIRED_FOOD) return false;

    return tile == ROAD_TILE || tile == FOOD_TILE || tile == ENDPOINT_TILE || tile == SNAKE_TILE || tile == DOG_TILE || tile == ZOMBIE_TILE;
}

bool isEndpointLocation(int col, int row)
{
    return static_cast<int>(gEndpointTile.x) == col && static_cast<int>(gEndpointTile.y) == row;
}

float getPlayerSpeed()
{
    int livesLost = STARTING_LIVES - gLives;
    float speed = BASE_MOVE_SPEED - FOOD_SLOWDOWN * gFoodCollected - LIFE_LOSS_SLOWDOWN * livesLost;

    if (speed < MIN_MOVE_SPEED) return MIN_MOVE_SPEED;

    return speed;
}

bool isColliding(const Vector2 *positionA, float scaleA, const Vector2 *positionB, float scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((TILE_SIZE * scaleA + TILE_SIZE * scaleB) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((TILE_SIZE * scaleA + TILE_SIZE * scaleB) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void updateCamera()
{
    gCamera.target = gPlayerPosition;

    float minTargetX = SCREEN_WIDTH / 2.0f;
    float minTargetY = SCREEN_HEIGHT / 2.0f;
    float maxTargetX = MAP_PIXEL_WIDTH - SCREEN_WIDTH / 2.0f;
    float maxTargetY = MAP_PIXEL_HEIGHT - SCREEN_HEIGHT / 2.0f;

    if (gCamera.target.x < minTargetX) gCamera.target.x = minTargetX;
    if (gCamera.target.y < minTargetY) gCamera.target.y = minTargetY;
    if (gCamera.target.x > maxTargetX) gCamera.target.x = maxTargetX;
    if (gCamera.target.y > maxTargetY) gCamera.target.y = maxTargetY;
}

void initialiseSnakes()
{
    gSnakeCount = 0;

    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            if (gLevelData[row][col] != SNAKE_TILE || gSnakeCount >= MAX_SNAKES) continue;

            int leftCol = col;
            int rightCol = col;

            while (leftCol > 0 && gLevelData[row][leftCol - 1] != WALL_TILE) leftCol--;
            while (rightCol < MAP_WIDTH - 1 && gLevelData[row][rightCol + 1] != WALL_TILE) rightCol++;

            gSnakes[gSnakeCount] = {
                {
                    static_cast<float>(MAP_OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2),
                    static_cast<float>(MAP_OFFSET_Y + row * TILE_SIZE + TILE_SIZE / 2)
                },
                static_cast<float>(MAP_OFFSET_X + leftCol * TILE_SIZE + TILE_SIZE / 2),
                static_cast<float>(MAP_OFFSET_X + rightCol * TILE_SIZE + TILE_SIZE / 2),
                -1
            };

            gSnakeCount++;
        }
    }
}

void initialiseZombies()
{
    gZombieCount = 0;

    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            if (gLevelData[row][col] != ZOMBIE_TILE || gZombieCount >= MAX_ZOMBIES) continue;

            int topRow = row;
            int bottomRow = row;

            while (topRow > 0 && gLevelData[topRow - 1][col] != WALL_TILE) topRow--;
            while (bottomRow < MAP_HEIGHT - 1 && gLevelData[bottomRow + 1][col] != WALL_TILE) bottomRow++;

            gZombies[gZombieCount] = {
                {
                    static_cast<float>(MAP_OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2),
                    static_cast<float>(MAP_OFFSET_Y + row * TILE_SIZE + TILE_SIZE / 2)
                },
                static_cast<float>(MAP_OFFSET_Y + topRow * TILE_SIZE + TILE_SIZE / 2),
                static_cast<float>(MAP_OFFSET_Y + bottomRow * TILE_SIZE + TILE_SIZE / 2),
                -1
            };

            gZombieCount++;
            gLevelData[row][col] = ROAD_TILE;
        }
    }
}

void updateSnakes(float deltaTime)
{
    if (gCurrentLevel == 2) return;

    for (int i = 0; i < gSnakeCount; i++)
    {
        gSnakes[i].position.x += SNAKE_SPEED * gSnakes[i].direction * deltaTime;

        if (gSnakes[i].position.x <= gSnakes[i].minX)
        {
            gSnakes[i].position.x = gSnakes[i].minX;
            gSnakes[i].direction = 1;
        }
        else if (gSnakes[i].position.x >= gSnakes[i].maxX)
        {
            gSnakes[i].position.x = gSnakes[i].maxX;
            gSnakes[i].direction = -1;
        }
    }
}

void checkSnakeCollisions(float deltaTime)
{
    if (gDamageTimer > 0.0f || gHasLost || gHasWon) return;

    for (int i = 0; i < gSnakeCount; i++)
    {
        if (!isColliding(&gPlayerPosition, GEORGE_SCALE, &gSnakes[i].position, SNAKE_SCALE)) continue;

        PlaySound(gSnakeBiteSound);
        gLives--;
        gDamageTimer = DAMAGE_COOLDOWN;

        if (gLives <= 0)
        {
            gLives = 0;
            gHasLost = true;
            gIsMoving = false;
        }

        return;
    }
}

void updateZombies(float deltaTime)
{
    for (int i = 0; i < gZombieCount; i++)
    {
        gZombies[i].position.y += ZOMBIE_SPEED * gZombies[i].direction * deltaTime;

        if (gZombies[i].position.y <= gZombies[i].minY)
        {
            gZombies[i].position.y = gZombies[i].minY;
            gZombies[i].direction = 1;
        }
        else if (gZombies[i].position.y >= gZombies[i].maxY)
        {
            gZombies[i].position.y = gZombies[i].maxY;
            gZombies[i].direction = -1;
        }
    }
}

void checkZombieCollisions(float deltaTime)
{
    if (gDamageTimer > 0.0f || gHasLost || gHasWon) return;

    for (int i = 0; i < gZombieCount; i++)
    {
        if (!isColliding(&gPlayerPosition, GEORGE_SCALE, &gZombies[i].position, ZOMBIE_SCALE)) continue;

        PlaySound(gZombieBiteSound);
        gLives--;
        gDamageTimer = DAMAGE_COOLDOWN;

        if (gLives <= 0)
        {
            gLives = 0;
            gHasLost = true;
            gIsMoving = false;
        }

        return;
    }
}

void initialiseDog()
{
    gDog = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, false, false, false, DOG_RIGHT, 0, 0.0f };
    gDogBarkPlayed = false;

    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            if (gLevelData[row][col] != DOG_TILE) continue;

            Vector2 dogPosition = {
                static_cast<float>(MAP_OFFSET_X + col * TILE_SIZE + TILE_SIZE / 2),
                static_cast<float>(MAP_OFFSET_Y + row * TILE_SIZE + TILE_SIZE / 2)
            };

            gDog = { dogPosition, dogPosition, true, false, false, DOG_RIGHT, 0, 0.0f };
            return;
        }
    }
}

bool isDogRoadPosition(const Vector2 *position)
{
    int col = static_cast<int>((position->x - MAP_OFFSET_X) / TILE_SIZE);
    int row = static_cast<int>((position->y - MAP_OFFSET_Y) / TILE_SIZE);

    if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT) return false;

    char tile = gLevelData[row][col];
    return tile == WALL_TILE || tile == ROAD_TILE || tile == FOOD_TILE || tile == ENDPOINT_TILE || tile == SNAKE_TILE || tile == DOG_TILE;
}

void updateDog(float deltaTime)
{
    if (!gDog.isOnMap || gDog.hasCaught || gHasWon || gHasLost) return;

    if (!gDog.isChasing)
    {
        if (gCurrentLevel == 1 && gFoodCollected >= 3)
        {
            gDog.isChasing = true;
            if (!gDogBarkPlayed)
            {
                PlaySound(gDogBarkSound);
                gDogBarkPlayed = true;
            }
        }

        return;
    }

    Vector2 direction = Vector2Subtract(gPlayerPosition, gDog.position);
    if (GetLength(direction) <= 1.0f) return;

    Normalise(&direction);

    Vector2 step = Vector2Scale(direction, DOG_SPEED * deltaTime);
    Vector2 nextPosition = Vector2Add(gDog.position, step);
    bool didMove = false;

    if (isDogRoadPosition(&nextPosition))
    {
        gDog.position = nextPosition;
        didMove = true;
    }
    else
    {
        Vector2 nextXPosition = { gDog.position.x + step.x, gDog.position.y };
        Vector2 nextYPosition = { gDog.position.x, gDog.position.y + step.y };

        if (fabs(step.x) > fabs(step.y))
        {
            if (isDogRoadPosition(&nextXPosition))
            {
                gDog.position = nextXPosition;
                didMove = true;
            }
            else if (isDogRoadPosition(&nextYPosition))
            {
                gDog.position = nextYPosition;
                didMove = true;
            }
        }
        else
        {
            if (isDogRoadPosition(&nextYPosition))
            {
                gDog.position = nextYPosition;
                didMove = true;
            }
            else if (isDogRoadPosition(&nextXPosition))
            {
                gDog.position = nextXPosition;
                didMove = true;
            }
        }
    }

    if (!didMove) return;

    if (step.x < -0.01f) gDog.direction = DOG_LEFT;
    else if (step.x > 0.01f) gDog.direction = DOG_RIGHT;

    gDog.frameTimer += deltaTime;
    if (gDog.frameTimer >= FRAME_TIME)
    {
        gDog.frame = (gDog.frame + 1) % DOG_WALK_FRAMES;
        gDog.frameTimer = 0.0f;
    }

}

void checkDogCollision()
{
    if (!gDog.isOnMap || !gDog.isChasing || gDog.hasCaught || gHasLost || gHasWon) return;
    if (!isColliding(&gPlayerPosition, GEORGE_SCALE, &gDog.position, DOG_SCALE)) return;

    if (gFoodCollected > 0) gFoodCollected--;
    PlaySound(gDogEatingSound);

    gDog.isChasing = false;
    gDog.hasCaught = true;
    gDog.frame = 0;
    gDog.frameTimer = 0.0f;
}

void loadLevel(int levelIndex)
{
    gCurrentLevel = levelIndex;
    int startCol = 1;
    int startRow = 1;
    int endpointCol = 1;
    int endpointRow = 1;
    bool foundStart = false;

    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH + 1; col++)
        {
            gLevelData[row][col] = LEVEL_MAPS[levelIndex][row][col];

            if (LEVEL_MAPS[levelIndex][row][col] == ENDPOINT_TILE)
            {
                if (!foundStart)
                {
                    startCol = col;
                    startRow = row;
                    foundStart = true;
                }

                endpointCol = col;
                endpointRow = row;
            }
        }
    }

    gEndpointTile = {
        static_cast<float>(endpointCol),
        static_cast<float>(endpointRow)
    };

    gPlayerTile = {
        static_cast<float>(startCol),
        static_cast<float>(startRow)
    };
    gPlayerPosition = {
        static_cast<float>(MAP_OFFSET_X + startCol * TILE_SIZE + TILE_SIZE / 2),
        static_cast<float>(MAP_OFFSET_Y + startRow * TILE_SIZE + TILE_SIZE / 2)
    };
    gPlayerTargetPosition = gPlayerPosition;

    gFoodCollected = 0;
    gIsMoving = false;
    gHasWon = false;
    gHasLost = false;
    gPlayerDirection = GEORGE_DOWN;
    gPlayerFrame = 0;
    gFrameTimer = 0.0f;
    gDamageTimer = 0.0f;

    initialiseSnakes();
    initialiseZombies();
    initialiseDog();
    updateCamera();
}

void resetGameState()
{
    gLives = STARTING_LIVES;
    gHasWon = false;
    gHasLost = false;
    gFoodCollected = 0;
    gIsMoving = false;
    gDamageTimer = 0.0f;
}

void tryMovePlayer(int xDirection, int yDirection)
{
    if (gHasWon || gHasLost || gIsMoving) return;

    if      (xDirection < 0) gPlayerDirection = GEORGE_LEFT;
    else if (xDirection > 0) gPlayerDirection = GEORGE_RIGHT;
    else if (yDirection < 0) gPlayerDirection = GEORGE_UP;
    else if (yDirection > 0) gPlayerDirection = GEORGE_DOWN;

    int nextCol = static_cast<int>(gPlayerTile.x) + xDirection;
    int nextRow = static_cast<int>(gPlayerTile.y) + yDirection;

    if (!isWalkableTile(nextCol, nextRow)) return;

    gPlayerTile = {
        static_cast<float>(nextCol),
        static_cast<float>(nextRow)
    };

    gPlayerTargetPosition = {
        static_cast<float>(MAP_OFFSET_X + nextCol * TILE_SIZE + TILE_SIZE / 2),
        static_cast<float>(MAP_OFFSET_Y + nextRow * TILE_SIZE + TILE_SIZE / 2)
    };

    gIsMoving = true;

    if (gLevelData[nextRow][nextCol] == FOOD_TILE)
    {
        gFoodCollected++;
        gLevelData[nextRow][nextCol] = ROAD_TILE;
    }

    if (isEndpointLocation(nextCol, nextRow))
    {
        if (gCurrentLevel < NUMBER_OF_LEVELS - 1)
            gCurrentScene->getState().nextSceneID = gCurrentLevel + 2;
        else
        {
            gHasWon = true;
            gIsMoving = false;
        }
    }
}

void initialiseGameAssets()
{
    gBackgroundMusic  = LoadMusicStream(BACKGROUND_MUSIC_FP);
    gRoadTexture      = LoadTexture(ROAD_FP);
    gWallTexture      = LoadTexture(WALL_FP);
    gEndpointTexture  = LoadTexture(ENDPOINT_FP);
    gCharacterTexture = LoadTexture(CHARACTER_FP);
    gFoodTexture      = LoadTexture(FOOD_FP);
    gSnakeTexture     = LoadTexture(SNAKE_FP);
    gZombieTexture    = LoadTexture(ZOMBIE_TILE_FP);
    gDogTexture       = LoadTexture(DOG_FP);
    gDogBarkSound     = LoadSound(DOG_BARK_FP);
    gDogEatingSound   = LoadSound(DOG_EATING_FP);
    gZombieSound      = LoadSound(ZOMBIE_FP);
    gZombieBiteSound  = LoadSound(ZOMBIE_BITE_FP);
    gSnakeBiteSound   = LoadSound(SNAKE_BITE_FP);
    gShader.load(VERTEX_SHADER_FP, FRAGMENT_SHADER_FP);

    SetMusicVolume(gBackgroundMusic, 0.33f);
    PlayMusicStream(gBackgroundMusic);

    SetTargetFPS(FPS);
}

void processGameplayInput()
{
    if      (IsKeyDown(KEY_A)) tryMovePlayer(-1,  0);
    else if (IsKeyDown(KEY_D)) tryMovePlayer( 1,  0);
    else if (IsKeyDown(KEY_W)) tryMovePlayer( 0, -1);
    else if (IsKeyDown(KEY_S)) tryMovePlayer( 0,  1);
}

void updateGameplay(float deltaTime)
{
    if (gDamageTimer > 0.0f) gDamageTimer -= deltaTime;

    updateSnakes(deltaTime);
    checkSnakeCollisions(deltaTime);
    updateZombies(deltaTime);
    checkZombieCollisions(deltaTime);
    updateDog(deltaTime);
    checkDogCollision();

    if (!gIsMoving)
    {
        updateCamera();
        return;
    }

    Vector2 distance = Vector2Subtract(gPlayerTargetPosition, gPlayerPosition);
    float distanceLength = GetLength(distance);
    float step = getPlayerSpeed() * deltaTime;

    if (distanceLength <= step)
    {
        gPlayerPosition = gPlayerTargetPosition;
        gIsMoving = false;
        gPlayerFrame = 0;
        updateCamera();
        return;
    }

    Normalise(&distance);
    gPlayerPosition = Vector2Add(
        gPlayerPosition,
        Vector2Scale(distance, step)
    );

    gFrameTimer += deltaTime;
    if (gFrameTimer >= FRAME_TIME)
    {
        gPlayerFrame++;
        if (gPlayerFrame >= GEORGE_ROWS) gPlayerFrame = 0;
        gFrameTimer = 0.0f;
    }

    updateCamera();
}

void renderGameplay()
{
    ClearBackground(ColorFromHex(BG_COLOUR));
    BeginMode2D(gCamera);

    if (gCurrentLevel == 2)
    {
        gShader.begin();
        gShader.setVector2("lightPosition", gPlayerPosition);
        gShader.setFloat("lightRadius", LIGHT_RADIUS);
        gShader.setInt("useCircleLight", 1);
    }

    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            char tile = gLevelData[row][col];

            if (tile == WALL_TILE)
                drawTextureAtTile(&gWallTexture, col, row, 1.0f);
            else if (tile == ENDPOINT_TILE)
                drawTextureAtTile(&gEndpointTexture, col, row, 1.0f);
            else
            {
                drawTextureAtTile(&gRoadTexture, col, row, 1.0f);
                if (tile == FOOD_TILE)
                    drawTextureAtTile(&gFoodTexture, col, row, 0.7f);
            }
        }
    }

    for (int i = 0; i < gSnakeCount; i++)
    {
        drawSnakeAtPosition(&gSnakes[i]);
    }

    for (int i = 0; i < gZombieCount; i++)
    {
        drawZombieAtPosition(&gZombies[i]);
    }

    if (gDog.isOnMap) drawDogAtPosition(&gDog);

    float playerScale = gHasWon ? GEORGE_SCALE * 1.15f : GEORGE_SCALE;
    drawGeorgeAtPosition(&gPlayerPosition, playerScale);

    if (gCurrentLevel == 2) gShader.end();

    EndMode2D();

    for (int i = 0; i < REQUIRED_FOOD; i++)
    {
        Texture2D *counterTexture = i < gFoodCollected ? &gFoodTexture : &gRoadTexture;
        Rectangle textureArea = {
            0.0f,
            0.0f,
            static_cast<float>(counterTexture->width),
            static_cast<float>(counterTexture->height)
        };

        Rectangle destinationArea = {
            24.0f + i * (TILE_SIZE / 2.0f + 10.0f),
            24.0f,
            TILE_SIZE / 2.0f,
            TILE_SIZE / 2.0f
        };

        Vector2 originOffset = { 0.0f, 0.0f };

        DrawTexturePro(
            *counterTexture,
            textureArea,
            destinationArea,
            originOffset,
            gAngle,
            WHITE
        );
    }

    for (int i = 0; i < STARTING_LIVES; i++)
    {
        Rectangle textureArea = {
            0.0f,
            0.0f,
            gCharacterTexture.width / (float) GEORGE_COLS,
            gCharacterTexture.height / (float) GEORGE_ROWS
        };

        Rectangle destinationArea = {
            24.0f + i * (TILE_SIZE / 2.0f + 10.0f),
            70.0f,
            TILE_SIZE / 2.0f,
            TILE_SIZE / 2.0f
        };

        Vector2 originOffset = { 0.0f, 0.0f };
        Color tint = i < gLives ? WHITE : ColorFromHex("#3B4656");

        DrawTexturePro(
            gCharacterTexture,
            textureArea,
            destinationArea,
            originOffset,
            gAngle,
            tint
        );
    }
}

void shutdownGameAssets()
{
    UnloadMusicStream(gBackgroundMusic);
    UnloadTexture(gRoadTexture);
    UnloadTexture(gWallTexture);
    UnloadTexture(gEndpointTexture);
    UnloadTexture(gCharacterTexture);
    UnloadTexture(gFoodTexture);
    UnloadTexture(gSnakeTexture);
    UnloadTexture(gZombieTexture);
    UnloadTexture(gDogTexture);
    UnloadSound(gDogBarkSound);
    UnloadSound(gDogEatingSound);
    UnloadSound(gZombieSound);
    UnloadSound(gZombieBiteSound);
    UnloadSound(gSnakeBiteSound);
    gShader.unload();
}

void LevelScene::initialise()
{
    mGameState.nextSceneID = -1;
    if (mLevelIndex == 0 || gShouldResetGame)
    {
        resetGameState();
        gShouldResetGame = false;
    }

    loadLevel(mLevelIndex);
    if (mLevelIndex == 2) PlaySound(gZombieSound);
}

void LevelScene::update(float deltaTime)
{
    updateGameplay(deltaTime);

    if (IsKeyPressed(KEY_R))
    {
        gShouldResetGame = true;
        mGameState.nextSceneID = LEVEL_ONE_SCENE_ID;
        return;
    }

    if ((gHasWon || gHasLost) && IsKeyPressed((KeyboardKey) SDLK_RETURN))
    {
        gShouldResetGame = true;
        mGameState.nextSceneID = MENU_SCENE_ID;
    }
}

void LevelScene::render()
{
    renderGameplay();

    if (!gHasWon && !gHasLost) return;

    const char *resultText = gHasWon
        ? "Congradulations! A rescue team foun your family."
        : "You died.";
    const char *promptText = gHasWon
        ? "Press R to restart or Enter to return to menu"
        : "Press R to restart or Enter to return to menu";
    int resultSize = 28;
    int promptSize = 26;

    DrawText(
        resultText,
        (SCREEN_WIDTH - MeasureText(resultText, resultSize)) / 2,
        110,
        resultSize,
        WHITE
    );

    DrawText(
        promptText,
        (SCREEN_WIDTH - MeasureText(promptText, promptSize)) / 2,
        160,
        promptSize,
        WHITE
    );
}

void LevelScene::shutdown()
{
}

void switchToScene(Scene *scene)
{
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();

    gCurrentScene = scene;
    gTimeAccumulator = 0.0f;
    gPreviousTicks = (float) GetTime();
    gCurrentScene->initialise();
}

void initialiseApp()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    InitAudioDevice();
    initialiseGameAssets();

    gMenuScene = new MenuScene(
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        MENU_BG_COLOUR
    );

    gLevelOneScene = new LevelScene(
        0,
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        BG_COLOUR
    );

    gLevelTwoScene = new LevelScene(
        1,
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        BG_COLOUR
    );

    gLevelThreeScene = new LevelScene(
        2,
        { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        BG_COLOUR
    );

    gCamera.offset   = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    switchToScene(gMenuScene);
    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose() || IsKeyPressed(KEY_Q))
    {
        gAppStatus = TERMINATED;
        return;
    }

    if (gCurrentScene == gLevelOneScene ||
        gCurrentScene == gLevelTwoScene ||
        gCurrentScene == gLevelThreeScene)
        processGameplayInput();
}

void updateApp()
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    UpdateMusicStream(gBackgroundMusic);

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
    }

    gTimeAccumulator = deltaTime;

    switch (gCurrentScene->getState().nextSceneID)
    {
        case MENU_SCENE_ID:
            if (gCurrentScene != gMenuScene) switchToScene(gMenuScene);
            break;
        case LEVEL_ONE_SCENE_ID:
            if (gCurrentScene != gLevelOneScene || gShouldResetGame) switchToScene(gLevelOneScene);
            break;
        case LEVEL_TWO_SCENE_ID:
            if (gCurrentScene != gLevelTwoScene) switchToScene(gLevelTwoScene);
            break;
        case LEVEL_THREE_SCENE_ID:
            if (gCurrentScene != gLevelThreeScene) switchToScene(gLevelThreeScene);
            break;
        default:
            break;
    }
}

void renderApp()
{
    BeginDrawing();

    if (gCurrentScene->usesCamera()) BeginMode2D(gCamera);
    gCurrentScene->render();
    if (gCurrentScene->usesCamera()) EndMode2D();

    EndDrawing();
}

void shutdownApp()
{
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();

    delete gMenuScene;
    delete gLevelOneScene;
    delete gLevelTwoScene;
    delete gLevelThreeScene;
    gMenuScene = nullptr;
    gLevelOneScene = nullptr;
    gLevelTwoScene = nullptr;
    gLevelThreeScene = nullptr;
    gCurrentScene = nullptr;

    shutdownGameAssets();
    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialiseApp();

    while (gAppStatus == RUNNING)
    {
        processInput();
        updateApp();
        renderApp();
    }

    shutdownApp();

    return 0;
}
