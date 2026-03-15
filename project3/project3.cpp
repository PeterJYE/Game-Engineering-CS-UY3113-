
#include "CS3113/Entity.h"

enum MissionStatus { IN_PROGRESS, MISSION_FAILED, MISSION_ACCOMPLISHED };

constexpr int SCREEN_WIDTH  = 960;
constexpr int SCREEN_HEIGHT = 540;
constexpr int FPS           = 60;

constexpr float FIXED_TIMESTEP            = 1.0f / 60.0f;
constexpr int   MAX_TIMESTEPS             = 6;
constexpr int   WORLD_COLLIDER_COUNT      = 6;
constexpr int   HAZARD_COUNT              = 2;
constexpr float GRAVITY_ACCELERATION      = 320.0f;
constexpr float HORIZONTAL_THRUST         = 520.0f;
constexpr float VERTICAL_THRUST           = 460.0f;
constexpr float HORIZONTAL_DRAG           = 300.0f;
constexpr float MAX_HORIZONTAL_SPEED      = 180.0f;
constexpr float MAX_VERTICAL_SPEED        = 260.0f;
constexpr float STARTING_FUEL             = 100.0f;
constexpr float FUEL_BURN_PER_SECOND      = 24.0f;
constexpr float PLAYER_TILT_ANGLE         = 18.0f;
constexpr float LANDING_PLATFORM_Y        = 300.0f;
constexpr float LANDING_PLATFORM_WIDTH    = 120.0f;
constexpr float LANDING_PLATFORM_HEIGHT   = 31.0f;
constexpr float LANDING_COLLIDER_WIDTH    = 120.0f;
constexpr float LANDING_COLLIDER_HEIGHT   = 31.0f;
constexpr float LANDING_MOVE_SPEED        = 120.0f;
constexpr float LANDING_AREA_MIN_X        = 30.0f;
constexpr float LANDING_AREA_MAX_X        = 300.0f;

constexpr char PLAYER_TEXTURE_FP[]   = "assets/textures/object.png";
constexpr char PLATFORM_TEXTURE_FP[] = "assets/textures/platform.png";
constexpr char FIRE_TEXTURE_FP[]     = "assets/textures/fire.png";

constexpr int   PLAYER_FALL_FRAME_COUNT = 5;
constexpr int   PLAYER_THRUST_FRAME_COUNT = 5;
constexpr int   PLAYER_DEATH_FRAME_COUNT = 8;
constexpr float PLAYER_ANIMATION_FRAME_TIME = 0.09f;
constexpr float PLAYER_FRAME_WIDTH = 64.0f;
constexpr float PLAYER_FRAME_HEIGHT = 64.0f;
constexpr int   FIRE_FRAME_COUNT = 60;
constexpr int   FIRE_COLUMNS = 10;
constexpr int   FIRE_ROWS = 6;
constexpr float FIRE_FRAME_WIDTH = 64.0f;
constexpr float FIRE_FRAME_HEIGHT = 64.0f;
constexpr float FIRE_ANIMATION_FRAME_TIME = 0.05f;
constexpr float FIRE_STRIP_HEIGHT = 28.0f;

enum PlayerAnimationState { PLAYER_FALLING, PLAYER_THRUSTING, PLAYER_DEAD };

enum ColliderIndex
{
    START_PLATFORM,
    MID_PLATFORM,
    SIDE_PLATFORM,
    LAVA_LEFT,
    LAVA_RIGHT,
    LANDING_PLATFORM
};

AppStatus gAppStatus = RUNNING;
MissionStatus gMissionStatus = IN_PROGRESS;

Entity *gPlayer = nullptr;
Entity *gWorldColliders = nullptr;
Texture2D gFireTexture = { 0 };

float gPreviousTicks = 0.0f;
float gAccumulator   = 0.0f;
float gElapsedTime   = 0.0f;
float gFuelRemaining = STARTING_FUEL;
float gPlayerAnimationTime = 0.0f;
float gFireAnimationTime = 0.0f;
float gLandingTargetX = 0.0f;

int gPlayerFrameIndex = 0;
int gFireFrameIndex = 0;
PlayerAnimationState gPlayerAnimationState = PLAYER_FALLING;

bool gLeftKeyDown  = false;
bool gRightKeyDown = false;
bool gThrustKeyDown = false;

void initialise();
void processInput();
void update(float deltaTime);
void render();
void shutdown();
void configureEntity(Entity *entity, Vector2 position, Vector2 scale, Vector2 colliderDimensions, EntityType type);
void applyPlayerAcceleration(float deltaTime);
void clampPlayerVelocity();
void clampPlayerToScreenTop();
void evaluateMissionState();
void updateLandingPlatform(float deltaTime);
void drawWorldDecor();
void drawFuelUI();
void updatePlayerAnimation(float deltaTime);
void drawPlayer();
void updateFireAnimation(float deltaTime);
void drawFireStrip();

void configureEntity(Entity *entity, Vector2 position, Vector2 scale, Vector2 colliderDimensions, EntityType type)
{
    entity->activate();
    entity->setPosition(position);
    entity->setScale(scale);
    entity->setColliderDimensions(colliderDimensions);
    entity->setTexture(PLATFORM_TEXTURE_FP);
    entity->setEntityType(type);
    entity->setVelocity({ 0.0f, 0.0f });
    entity->setAcceleration({ 0.0f, 0.0f });
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Project 3 - Lunar Rescue");
    SetTargetFPS(FPS);

    gPlayer = new Entity();
    gPlayer->setTexture(PLAYER_TEXTURE_FP);
    gPlayer->setEntityType(PLAYER);
    gPlayer->setPosition({ 110.0f, 120.0f });
    gPlayer->setScale({ 54.0f, 72.0f });
    gPlayer->setColliderDimensions({ 30.0f, 44.0f });
    gPlayer->setAcceleration({ 0.0f, GRAVITY_ACCELERATION });
    gPlayer->setVelocity({ 0.0f, 0.0f });
    gPlayer->setAngle(0.0f);
    gFireTexture = LoadTexture(FIRE_TEXTURE_FP);

    gWorldColliders = new Entity[WORLD_COLLIDER_COUNT];
    configureEntity(&gWorldColliders[START_PLATFORM], { 130.0f, 420.0f }, { 190.0f, 30.0f }, { 170.0f, 16.0f }, PLATFORM);
    configureEntity(&gWorldColliders[MID_PLATFORM],   { 360.0f, 315.0f }, { 150.0f, 28.0f }, { 134.0f, 16.0f }, PLATFORM);
    configureEntity(&gWorldColliders[SIDE_PLATFORM],  { 825.0f, 355.0f }, { 150.0f, 28.0f }, { 134.0f, 16.0f }, PLATFORM);
    configureEntity(&gWorldColliders[LAVA_LEFT],      { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, PLATFORM);
    configureEntity(&gWorldColliders[LAVA_RIGHT],     { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f }, PLATFORM);
    configureEntity(
        &gWorldColliders[LANDING_PLATFORM],
        { 800.0f, LANDING_PLATFORM_Y },
        { LANDING_PLATFORM_WIDTH, LANDING_PLATFORM_HEIGHT },
        { LANDING_COLLIDER_WIDTH, LANDING_COLLIDER_HEIGHT },
        PLATFORM
    );

    gWorldColliders[START_PLATFORM].deactivate();
    gWorldColliders[MID_PLATFORM].deactivate();
    gWorldColliders[SIDE_PLATFORM].deactivate();
    gWorldColliders[LAVA_LEFT].deactivate();
    gWorldColliders[LAVA_RIGHT].deactivate();
    gLandingTargetX = 240.0f;

    gPreviousTicks = (float)GetTime();
}

void processInput()
{
    gLeftKeyDown   = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    gRightKeyDown  = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    gThrustKeyDown = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);

    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void applyPlayerAcceleration(float deltaTime)
{
    Vector2 currentVelocity = gPlayer->getVelocity();
    Vector2 newAcceleration = { 0.0f, GRAVITY_ACCELERATION };
    bool controlUsed = false;

    if (gFuelRemaining > 0.0f)
    {
        if (gLeftKeyDown && !gRightKeyDown)
        {
            newAcceleration.x = -HORIZONTAL_THRUST;
            controlUsed = true;
            gPlayer->setAngle(-PLAYER_TILT_ANGLE);
        }
        else if (gRightKeyDown && !gLeftKeyDown)
        {
            newAcceleration.x = HORIZONTAL_THRUST;
            controlUsed = true;
            gPlayer->setAngle(PLAYER_TILT_ANGLE);
        }
        else
        {
            if (fabsf(currentVelocity.x) > 3.0f)
                newAcceleration.x = (currentVelocity.x > 0.0f) ? -HORIZONTAL_DRAG : HORIZONTAL_DRAG;

            gPlayer->setAngle(currentVelocity.x * 0.08f);
        }

        if (gThrustKeyDown)
        {
            newAcceleration.y -= VERTICAL_THRUST;
            controlUsed = true;
        }

        if (controlUsed)
        {
            gFuelRemaining -= FUEL_BURN_PER_SECOND * deltaTime;
            if (gFuelRemaining < 0.0f) gFuelRemaining = 0.0f;
        }
    }
    else
    {
        if (fabsf(currentVelocity.x) > 3.0f)
            newAcceleration.x = (currentVelocity.x > 0.0f) ? -HORIZONTAL_DRAG : HORIZONTAL_DRAG;

        gPlayer->setAngle(currentVelocity.x * 0.08f);
    }

    gPlayer->setAcceleration(newAcceleration);
}

void clampPlayerVelocity()
{
    Vector2 velocity = gPlayer->getVelocity();

    if (velocity.x > MAX_HORIZONTAL_SPEED) velocity.x = MAX_HORIZONTAL_SPEED;
    if (velocity.x < -MAX_HORIZONTAL_SPEED) velocity.x = -MAX_HORIZONTAL_SPEED;
    if (velocity.y > MAX_VERTICAL_SPEED) velocity.y = MAX_VERTICAL_SPEED;
    if (velocity.y < -MAX_VERTICAL_SPEED) velocity.y = -MAX_VERTICAL_SPEED;

    gPlayer->setVelocity(velocity);
}

void clampPlayerToScreenTop()
{
    Vector2 position = gPlayer->getPosition();
    Vector2 scale = gPlayer->getScale();

    float topEdge = position.y - scale.y / 2.0f;
    if (topEdge < 0.0f)
    {
        position.y = scale.y / 2.0f;
        gPlayer->setPosition(position);

        Vector2 velocity = gPlayer->getVelocity();
        if (velocity.y < 0.0f) velocity.y = 0.0f;
        gPlayer->setVelocity(velocity);
    }
}

void updateLandingPlatform(float deltaTime)
{
    Entity *platform = &gWorldColliders[LANDING_PLATFORM];
    Vector2 position = platform->getPosition();

    if (fabsf(position.x - gLandingTargetX) < LANDING_MOVE_SPEED * deltaTime)
    {
        position.x = gLandingTargetX;
        gLandingTargetX = (float)GetRandomValue((int)LANDING_AREA_MIN_X, (int)LANDING_AREA_MAX_X);
    }
    else
    {
        float direction = (gLandingTargetX > position.x) ? 1.0f : -1.0f;
        position.x += direction * LANDING_MOVE_SPEED * deltaTime;
    }

    platform->setPosition(position);
}

void updatePlayerAnimation(float deltaTime)
{
    PlayerAnimationState nextAnimationState = PLAYER_FALLING;

    if (gMissionStatus == MISSION_FAILED)
    {
        nextAnimationState = PLAYER_DEAD;
    }
    else if (gThrustKeyDown && gFuelRemaining > 0.0f)
    {
        nextAnimationState = PLAYER_THRUSTING;
    }

    if (nextAnimationState != gPlayerAnimationState)
    {
        gPlayerAnimationState = nextAnimationState;
        gPlayerFrameIndex = 0;
        gPlayerAnimationTime = 0.0f;
    }

    int frameCount = PLAYER_FALL_FRAME_COUNT;
    if (gPlayerAnimationState == PLAYER_THRUSTING) frameCount = PLAYER_THRUST_FRAME_COUNT;
    if (gPlayerAnimationState == PLAYER_DEAD) frameCount = PLAYER_DEATH_FRAME_COUNT;

    gPlayerAnimationTime += deltaTime;
    if (gPlayerAnimationTime >= PLAYER_ANIMATION_FRAME_TIME)
    {
        gPlayerAnimationTime = 0.0f;
        gPlayerFrameIndex++;

        if (gPlayerAnimationState == PLAYER_DEAD)
        {
            if (gPlayerFrameIndex >= frameCount)
            {
                gPlayer->deactivate();
                gPlayerFrameIndex = frameCount - 1;
            }
        }
        else
        {
            gPlayerFrameIndex %= frameCount;
        }
    }
}

void updateFireAnimation(float deltaTime)
{
    gFireAnimationTime += deltaTime;
    if (gFireAnimationTime >= FIRE_ANIMATION_FRAME_TIME)
    {
        gFireAnimationTime = 0.0f;
        gFireFrameIndex = (gFireFrameIndex + 1) % FIRE_FRAME_COUNT;
    }
}

void evaluateMissionState()
{
    Vector2 playerPosition = gPlayer->getPosition();
    Vector2 playerCollider = gPlayer->getColliderDimensions();
    float playerBottom = playerPosition.y + playerCollider.y / 2.0f;
    float fireTop = SCREEN_HEIGHT - FIRE_STRIP_HEIGHT;

    if (playerBottom >= fireTop)
    {
        gMissionStatus = MISSION_FAILED;
        return;
    }

    Vector2 platformPosition = gWorldColliders[LANDING_PLATFORM].getPosition();
    Vector2 platformCollider = gWorldColliders[LANDING_PLATFORM].getColliderDimensions();
    float xDistance = fabsf(playerPosition.x - platformPosition.x);
    float allowedXDistance = (playerCollider.x + platformCollider.x) / 2.0f;
    float platformTop = platformPosition.y - platformCollider.y / 2.0f;

    if (gPlayer->isCollidingBottom() &&
        xDistance <= allowedXDistance &&
        fabsf(playerBottom - platformTop) <= 2.0f)
    {
        gMissionStatus = MISSION_ACCOMPLISHED;
        return;
    }

    if (gPlayer->getPosition().y > SCREEN_HEIGHT + 40.0f ||
        gPlayer->getPosition().x < -40.0f ||
        gPlayer->getPosition().x > SCREEN_WIDTH + 40.0f ||
        gPlayer->getPosition().y < -60.0f)
    {
        gMissionStatus = MISSION_FAILED;
    }
}

void update(float deltaTime)
{
    updatePlayerAnimation(deltaTime);
    updateFireAnimation(deltaTime);
    if (gMissionStatus != IN_PROGRESS) return;

    gElapsedTime += deltaTime;
    updateLandingPlatform(deltaTime);
    applyPlayerAcceleration(deltaTime);

    gPlayer->update(deltaTime, gPlayer, gWorldColliders, WORLD_COLLIDER_COUNT, nullptr, 0);
    clampPlayerVelocity();
    clampPlayerToScreenTop();

    evaluateMissionState();
}

void drawWorldDecor()
{
    DrawCircle(120, 90, 38.0f, ColorFromHex("#FCE38A"));
    DrawCircle(120, 90, 54.0f, Fade(ColorFromHex("#FCE38A"), 0.20f));

    DrawRectangle(0, SCREEN_HEIGHT - FIRE_STRIP_HEIGHT, SCREEN_WIDTH, FIRE_STRIP_HEIGHT, ColorFromHex("#E85D04"));
    drawFireStrip();

}

void drawFireStrip()
{
    int sourceColumn = gFireFrameIndex % FIRE_COLUMNS;
    int sourceRow = (gFireFrameIndex / FIRE_COLUMNS) % FIRE_ROWS;

    Rectangle source = {
        sourceColumn * FIRE_FRAME_WIDTH,
        sourceRow * FIRE_FRAME_HEIGHT,
        FIRE_FRAME_WIDTH,
        FIRE_FRAME_HEIGHT
    };

    for (float x = 0.0f; x < SCREEN_WIDTH; x += FIRE_FRAME_WIDTH)
    {
        Rectangle destination = {
            x,
            SCREEN_HEIGHT - FIRE_STRIP_HEIGHT - 18.0f,
            FIRE_FRAME_WIDTH,
            FIRE_FRAME_HEIGHT
        };

        DrawTexturePro(gFireTexture, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
    }
}

void drawFuelUI()
{
    DrawText("Energy", 24, 20, 24, WHITE);
    float fuelRatio = gFuelRemaining / STARTING_FUEL;
    if (fuelRatio < 0.0f) fuelRatio = 0.0f;

    Color energyColor = (fuelRatio > 0.5f) ? ColorFromHex("#8BE28B") :
                        (fuelRatio > 0.2f) ? ColorFromHex("#F6BD60") :
                                             ColorFromHex("#F94144");

    DrawText(TextFormat("%i%%", (int)gFuelRemaining), 120, 20, 24, energyColor);
    DrawText("A/D steer, W thrust", 24, 58, 20, Fade(WHITE, 0.9f));
    DrawText("Land on the platform. Avoid the fire.", 24, 84, 20, Fade(WHITE, 0.9f));
}

void drawPlayer()
{
    if (!gPlayer->isActive()) return;

    Texture2D texture = gPlayer->getTexture();
    int frameCount = PLAYER_FALL_FRAME_COUNT;
    int rowIndex = 0;

    if (gPlayerAnimationState == PLAYER_THRUSTING)
    {
        rowIndex = 1;
        frameCount = PLAYER_THRUST_FRAME_COUNT;
    }
    else if (gPlayerAnimationState == PLAYER_DEAD)
    {
        rowIndex = 2;
        frameCount = PLAYER_DEATH_FRAME_COUNT;
    }

    Rectangle source = {
        PLAYER_FRAME_WIDTH * gPlayerFrameIndex,
        PLAYER_FRAME_HEIGHT * rowIndex,
        PLAYER_FRAME_WIDTH,
        PLAYER_FRAME_HEIGHT
    };

    Vector2 position = gPlayer->getPosition();
    Vector2 scale = gPlayer->getScale();
    Rectangle destination = {
        position.x,
        position.y,
        scale.x,
        scale.y
    };
    Vector2 origin = { scale.x / 2.0f, scale.y / 2.0f };

    DrawTexturePro(texture, source, destination, origin, gPlayer->getAngle(), WHITE);
}

void render()
{
    BeginDrawing();

    ClearBackground(ColorFromHex("#1D4ED8"));
    drawWorldDecor();

    for (int i = 0; i < WORLD_COLLIDER_COUNT; i++)
    {
        if (!gWorldColliders[i].isActive()) continue;

        if (i == LAVA_LEFT || i == LAVA_RIGHT)
        {
            Vector2 position = gWorldColliders[i].getPosition();
            Vector2 collider = gWorldColliders[i].getColliderDimensions();

            DrawRectangleRounded(
                {
                    position.x - collider.x / 2.0f,
                    position.y - collider.y / 2.0f,
                    collider.x,
                    collider.y
                },
                0.15f,
                6,
                Fade(ColorFromHex("#FF7B54"), 0.85f)
            );
        }
        else if (i != LANDING_PLATFORM)
        {
            Vector2 position = gWorldColliders[i].getPosition();
            Vector2 collider = gWorldColliders[i].getColliderDimensions();

            DrawRectangleRounded(
                {
                    position.x - collider.x / 2.0f,
                    position.y - collider.y / 2.0f,
                    collider.x,
                    collider.y
                },
                0.2f,
                6,
                Fade(ColorFromHex("#A3B18A"), 0.70f)
            );
        }

        gWorldColliders[i].render();
    }

    drawPlayer();
    drawFuelUI();

    if (gMissionStatus == MISSION_FAILED)
    {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.35f));
        DrawText("YOU DIED !!!", 315, 225, 52, ColorFromHex("#FFB4A2"));
    }
    else if (gMissionStatus == MISSION_ACCOMPLISHED)
    {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.25f));
        DrawText("YOU SURVIVED!!!", 270, 225, 52, ColorFromHex("#C7F9CC"));
    }

    EndDrawing();
}

void shutdown()
{
    delete gPlayer;
    gPlayer = nullptr;

    delete[] gWorldColliders;
    gWorldColliders = nullptr;

    UnloadTexture(gFireTexture);

    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();

        float ticks = (float)GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks = ticks;

        if (deltaTime > FIXED_TIMESTEP * MAX_TIMESTEPS)
            deltaTime = FIXED_TIMESTEP * MAX_TIMESTEPS;

        gAccumulator += deltaTime;

        while (gAccumulator >= FIXED_TIMESTEP)
        {
            update(FIXED_TIMESTEP);
            gAccumulator -= FIXED_TIMESTEP;
        }

        render();
    }

    shutdown();
    return 0;
}
