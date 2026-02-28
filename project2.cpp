/**

* Author: Peter Ye

* Assignment: Pong Clone

* Date due: 02/28/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/
#include "raylib.h"
#include <math.h>

// Enums
enum AppStatus { TERMINATED, RUNNING };
enum ShipStatus { GOING_RIGHT, GOING_LEFT, TURNING_AROUND };
enum WinnerStatus { NO_WINNER, BOMBER_WINNER, CAPTAIN_WINNER };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60;
constexpr int MAX_MISSILES  = 3;

constexpr char SUN_FP[]   = "assets/textures/sun.png";
constexpr char RIVER_FP[] = "assets/textures/river.png";
constexpr char BOAT_FP[]  = "assets/textures/boat.png";
constexpr char MISSILE_FP[] = "assets/textures/missile.png";
constexpr char SPLASH_FP[] = "assets/textures/Splash.png";
constexpr char EXPLOSION_FP[] = "assets/textures/explosion_sp.png";
constexpr char REFLECTER_FP[] = "assets/textures/reflecter.png";

// Global Variables
AppStatus gAppStatus = RUNNING;

Texture2D gSunTexture;
Texture2D gRiverTexture;
Texture2D gBoatTexture;
Texture2D gMissileTexture;
Texture2D gSplashTexture;
Texture2D gExplosionTexture;
Texture2D gReflecterTexture;

float gPreviousTicks = 0.0f;
float gTime          = 0.0f;

Vector2 gSunPosition   = { SCREEN_WIDTH * 0.85f, SCREEN_HEIGHT * 0.16f };
Vector2 gRiverPosition = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.88f };
Vector2 gBoatPosition  = { -55.0f, SCREEN_HEIGHT * 0.78f };
Vector2 gReflecterPosition = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.60f };
Vector2 gSplashPosition = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.78f };
Vector2 gExplosionPosition = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.78f };
Vector2 gBombPosition = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.78f };

Vector2 gSunScale   = { 100.0f, 100.0f };
Vector2 gRiverScale = { 860.0f, 90.0f };
Vector2 gBoatScale  = { 165.0f, 210.0f };
Vector2 gBoatHitboxScale = { 148.0f, 110.0f };
Vector2 gReflecterScale = { 90.0f, 28.0f };
Vector2 gReflecterHitboxScale = { 58.0f, 8.0f };
Vector2 gMissileScale = { 60.0f, 80.0f };
Vector2 gMissileHeadScale = { 14.0f, 14.0f };
Vector2 gSplashScale = { 84.0f, 84.0f };
Vector2 gExplosionScale = { 120.0f, 120.0f };
Vector2 gBombScale = { 12.0f, 16.0f };

float gSunAngle = 0.0f;
ShipStatus gShipStatus = GOING_RIGHT;
ShipStatus gShipStatusAfterTurn = GOING_LEFT;
float gBoatAngle = 0.0f;
float gBoatTargetAngle = 0.0f;
float gMissileAimOffset = 0.0f;
Vector2 gMissilePositions[MAX_MISSILES] = {
    { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f },
    { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f },
    { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f }
};
Vector2 gMissileMovements[MAX_MISSILES] = {
    { 0.0f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.0f }
};
float gMissileTravelAngles[MAX_MISSILES] = { 100.0f, 100.0f, 100.0f };
float gMissileDrawAngles[MAX_MISSILES] = { 240.0f, 240.0f, 240.0f };
bool gMissileIsLaunched[MAX_MISSILES] = { false, false, false };
bool gMissileIsVisible[MAX_MISSILES] = { true, false, false };
Vector2 gSplashPositions[MAX_MISSILES] = {
    { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.78f },
    { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.78f },
    { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.78f }
};
bool gSplashIsVisible[MAX_MISSILES] = { false, false, false };
float gSplashAnimationTimes[MAX_MISSILES] = { 0.0f, 0.0f, 0.0f };
int gSplashFrames[MAX_MISSILES] = { 0, 0, 0 };
bool gExplosionIsVisible = false;
float gExplosionAnimationTime = 0.0f;
int gExplosionFrame = 0;
bool gBoatIsVisible = true;
bool gBombIsVisible = false;
float gBombAngle = 0.0f;
bool gBoatAutoMove = false;
int gSelectedMissileCount = 1;
int gMissilesLeft = 3;
WinnerStatus gWinnerStatus = NO_WINNER;
float gCaptainWinTextSize = 8.0f;

void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *scale, float angle);
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);
void Win_Rotation(float deltaTime);

void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *scale, float angle)
{
    Rectangle textureArea = {
        0.0f,
        0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x,
        position->y,
        scale->x,
        scale->y
    };

    Vector2 objectOrigin = {
        scale->x / 2.0f,
        scale->y / 2.0f
    };

    DrawTexturePro(*texture, textureArea, destinationArea, objectOrigin, angle, WHITE);
}

bool isColliding(const Vector2 *positionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = positionA->x - positionB->x;
    float yDistance = positionA->y - positionB->y;

    if (xDistance < 0.0f) xDistance = -xDistance;
    if (yDistance < 0.0f) yDistance = -yDistance;

    xDistance -= (scaleA->x + scaleB->x) / 2.0f;
    yDistance -= (scaleA->y + scaleB->y) / 2.0f;

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void Win_Rotation(float deltaTime)
{
    if (gWinnerStatus == CAPTAIN_WINNER)
    {
        gBoatAngle += 220.0f * deltaTime;
        if (gBoatAngle >= 360.0f) gBoatAngle -= 360.0f;

        gCaptainWinTextSize += 45.0f * deltaTime;
        if (gCaptainWinTextSize > 36.0f) gCaptainWinTextSize = 36.0f;
    }

    if (gWinnerStatus == BOMBER_WINNER && gBombIsVisible)
    {
        gBombAngle += 180.0f * deltaTime;
        if (gBombAngle >= 360.0f) gBombAngle -= 360.0f;
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battleship");

    gSunTexture   = LoadTexture(SUN_FP);
    gRiverTexture = LoadTexture(RIVER_FP);
    gBoatTexture  = LoadTexture(BOAT_FP);
    gMissileTexture = LoadTexture(MISSILE_FP);
    gSplashTexture = LoadTexture(SPLASH_FP);
    gExplosionTexture = LoadTexture(EXPLOSION_FP);
    gReflecterTexture = LoadTexture(REFLECTER_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
    constexpr float MISSILE_AIM_STEP = 2.0f;
    constexpr float MISSILE_MAX_AIM_OFFSET = 65.0f;
    bool hasFlyingMissile = false;
    bool hasSplash = false;

    if (gWinnerStatus != NO_WINNER)
    {
        if (WindowShouldClose()) gAppStatus = TERMINATED;
        return;
    }

    if (IsKeyPressed(KEY_T)) gBoatAutoMove = !gBoatAutoMove;

    if (!gBoatAutoMove && IsKeyPressed(KEY_Q) && gShipStatus != TURNING_AROUND)
    {
        gShipStatusAfterTurn = (gShipStatus == GOING_RIGHT) ? GOING_LEFT : GOING_RIGHT;
        gShipStatus = TURNING_AROUND;
        gBoatTargetAngle += 180.0f;
        if (gBoatTargetAngle >= 360.0f) gBoatTargetAngle -= 360.0f;
    }

    if (IsKeyDown(KEY_RIGHT)) gMissileAimOffset -= MISSILE_AIM_STEP;
    if (IsKeyDown(KEY_LEFT)) gMissileAimOffset += MISSILE_AIM_STEP;
    if (gMissileAimOffset < -MISSILE_MAX_AIM_OFFSET) gMissileAimOffset = -MISSILE_MAX_AIM_OFFSET;
    if (gMissileAimOffset > MISSILE_MAX_AIM_OFFSET) gMissileAimOffset = MISSILE_MAX_AIM_OFFSET;

    if (IsKeyDown(KEY_N)) gReflecterPosition.x -= 4.0f;
    if (IsKeyDown(KEY_M)) gReflecterPosition.x += 4.0f;

    if (gReflecterPosition.x < gReflecterScale.x / 2.0f) gReflecterPosition.x = gReflecterScale.x / 2.0f;
    if (gReflecterPosition.x > SCREEN_WIDTH - (gReflecterScale.x / 2.0f))
        gReflecterPosition.x = SCREEN_WIDTH - (gReflecterScale.x / 2.0f);

    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (gMissileIsLaunched[i]) hasFlyingMissile = true;
        if (gSplashIsVisible[i]) hasSplash = true;
    }

    if (IsKeyPressed(KEY_ONE)) gSelectedMissileCount = 1;
    if (IsKeyPressed(KEY_TWO)) gSelectedMissileCount = 2;
    if (IsKeyPressed(KEY_THREE)) gSelectedMissileCount = 3;
    if (gSelectedMissileCount > gMissilesLeft) gSelectedMissileCount = gMissilesLeft;

    if (!hasFlyingMissile && !hasSplash && !gExplosionIsVisible && !gBombIsVisible)
    {
        float aimAngles[MAX_MISSILES] = {
            100.0f + gMissileAimOffset,
            100.0f + gMissileAimOffset - 20.0f,
            100.0f + gMissileAimOffset + 20.0f
        };

        if (gSelectedMissileCount == 1)
        {
            aimAngles[1] = 100.0f + gMissileAimOffset;
            aimAngles[2] = 100.0f + gMissileAimOffset;
        }

        if (gSelectedMissileCount == 2)
        {
            aimAngles[2] = 100.0f + gMissileAimOffset;
        }

        for (int i = 0; i < MAX_MISSILES; i++)
        {
            gMissileTravelAngles[i] = aimAngles[i];
            gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;
            gMissilePositions[i] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f };
            gMissileIsVisible[i] = (i < gSelectedMissileCount && i < gMissilesLeft);
        }
    }

    if (IsKeyPressed(KEY_SPACE) && !hasFlyingMissile && !hasSplash && !gExplosionIsVisible && !gBombIsVisible && gSelectedMissileCount > 0)
    {
        const float MISSILE_LAUNCH_SPEED = 250.0f;
        float launchAngles[MAX_MISSILES] = {
            100.0f + gMissileAimOffset,
            100.0f + gMissileAimOffset - 20.0f,
            100.0f + gMissileAimOffset + 20.0f
        };

        if (gSelectedMissileCount == 1)
        {
            launchAngles[1] = 100.0f + gMissileAimOffset;
            launchAngles[2] = 100.0f + gMissileAimOffset;
        }

        if (gSelectedMissileCount == 2)
        {
            launchAngles[2] = 100.0f + gMissileAimOffset;
        }

        for (int i = 0; i < MAX_MISSILES; i++)
        {
            float launchAngle = launchAngles[i] * PI / 180.0f;

            gMissilePositions[i] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f };
            gMissileTravelAngles[i] = launchAngles[i];
            gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;
            gMissileMovements[i] = {
                MISSILE_LAUNCH_SPEED * cos(launchAngle),
                MISSILE_LAUNCH_SPEED * cos(launchAngle - (PI / 2.0f))
            };
            gMissileIsLaunched[i] = (i < gSelectedMissileCount);
            gMissileIsVisible[i] = (i < gSelectedMissileCount);
            gSplashIsVisible[i] = false;
            gSplashAnimationTimes[i] = 0.0f;
            gSplashFrames[i] = 0;
        }

        gMissilesLeft -= gSelectedMissileCount;
        if (gMissilesLeft < 0) gMissilesLeft = 0;
        if (gSelectedMissileCount > gMissilesLeft) gSelectedMissileCount = gMissilesLeft;
    }

    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{

    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTime += deltaTime;

    // Sun
    const Vector2 sunAnchor = { SCREEN_WIDTH * 0.86f, SCREEN_HEIGHT * 0.16f };
    gSunPosition = {
        sunAnchor.x + 25.0f * cos(gTime * 0.9f),
        sunAnchor.y + 14.0f * cos((gTime * 0.9f) - (PI / 2.0f))
    };
    gSunAngle += 55.0f * deltaTime;

    // River
    const Vector2 riverAnchor = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.88f };
    gRiverPosition = {
        riverAnchor.x + 8.0f * cos(gTime * 0.5f),
        riverAnchor.y + 6.0f * cos((gTime * 0.8f) - (PI / 2.0f))
    };
    gRiverScale = {
        860.0f + 18.0f * cos((gTime * 1.3f) - (PI / 2.0f)),
        90.0f + 5.0f * cos(gTime * 1.1f)
    };

    // Boat controls
    float BOAT_FORWARD_SPEED = 250.0f;
    float BOAT_BACKUP_SPEED  = 120.0f;
    float horizontalVelocity = 0.0f;

    if (gBoatAutoMove)
    {
        if (gShipStatus == GOING_RIGHT) horizontalVelocity = BOAT_FORWARD_SPEED;
        else if (gShipStatus == GOING_LEFT) horizontalVelocity = -BOAT_FORWARD_SPEED;
    }
    else if (gShipStatus == TURNING_AROUND)
    {
        if (IsKeyDown(KEY_A)) horizontalVelocity = -BOAT_BACKUP_SPEED;
        else if (IsKeyDown(KEY_D)) horizontalVelocity = BOAT_BACKUP_SPEED;
    }
    else if (gShipStatus == GOING_RIGHT)
    {
        if (IsKeyDown(KEY_A)) horizontalVelocity = -BOAT_BACKUP_SPEED;
        else if (IsKeyDown(KEY_D)) horizontalVelocity = BOAT_FORWARD_SPEED;
    }
    else
    {
        if (IsKeyDown(KEY_A)) horizontalVelocity = -BOAT_FORWARD_SPEED;
        else if (IsKeyDown(KEY_D)) horizontalVelocity = BOAT_BACKUP_SPEED;
    }

    gBoatPosition.x += horizontalVelocity * deltaTime;

    float halfBoatWidth = gBoatScale.x / 2.0f;
    if (gBoatPosition.x < halfBoatWidth) gBoatPosition.x = halfBoatWidth;
    if (gBoatPosition.x > SCREEN_WIDTH - halfBoatWidth) gBoatPosition.x = SCREEN_WIDTH - halfBoatWidth;
    gBoatPosition.y = gRiverPosition.y - (gRiverScale.y / 2.0f);

    if (gBoatAutoMove && gShipStatus != TURNING_AROUND)
    {
        if (gBoatPosition.x <= halfBoatWidth)
        {
            gShipStatusAfterTurn = GOING_RIGHT;
            gShipStatus = TURNING_AROUND;
            gBoatTargetAngle += 180.0f;
            if (gBoatTargetAngle >= 360.0f) gBoatTargetAngle -= 360.0f;
        }
        else if (gBoatPosition.x >= SCREEN_WIDTH - halfBoatWidth)
        {
            gShipStatusAfterTurn = GOING_LEFT;
            gShipStatus = TURNING_AROUND;
            gBoatTargetAngle += 180.0f;
            if (gBoatTargetAngle >= 360.0f) gBoatTargetAngle -= 360.0f;
        }
    }

    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (gMissileIsLaunched[i])
        {
            const float MISSILE_LAUNCH_SPEED = 250.0f;
            float riverTop = gRiverPosition.y - (gRiverScale.y / 2.0f);
            float halfMissileWidth = gMissileScale.x / 2.0f;
            Vector2 boatHitboxPosition = { gBoatPosition.x, gBoatPosition.y + 38.0f };
            gMissilePositions[i] = {
                gMissilePositions[i].x + gMissileMovements[i].x * deltaTime,
                gMissilePositions[i].y + gMissileMovements[i].y * deltaTime
            };

            float missileAngleRadians = gMissileTravelAngles[i] * PI / 180.0f;
            Vector2 missileHeadPosition = {
                gMissilePositions[i].x + (gMissileScale.y / 2.0f - 8.0f) * cos(missileAngleRadians),
                gMissilePositions[i].y + (gMissileScale.y / 2.0f - 8.0f) * cos(missileAngleRadians - (PI / 2.0f))
            };
            Vector2 reflecterHitboxPosition = { gReflecterPosition.x, gReflecterPosition.y + 2.0f };

            if (gMissilePositions[i].x <= halfMissileWidth || gMissilePositions[i].x >= SCREEN_WIDTH - halfMissileWidth)
            {
                if (gMissilePositions[i].x < halfMissileWidth) gMissilePositions[i].x = halfMissileWidth;
                if (gMissilePositions[i].x > SCREEN_WIDTH - halfMissileWidth) gMissilePositions[i].x = SCREEN_WIDTH - halfMissileWidth;

                gMissileTravelAngles[i] = 180.0f - gMissileTravelAngles[i];
                gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;

                if (gMissileTravelAngles[i] < 0.0f) gMissileTravelAngles[i] += 360.0f;
                if (gMissileTravelAngles[i] >= 360.0f) gMissileTravelAngles[i] -= 360.0f;
                if (gMissileDrawAngles[i] >= 360.0f) gMissileDrawAngles[i] -= 360.0f;

                gMissileMovements[i] = {
                    MISSILE_LAUNCH_SPEED * cos(gMissileTravelAngles[i] * PI / 180.0f),
                    MISSILE_LAUNCH_SPEED * cos((gMissileTravelAngles[i] * PI / 180.0f) - (PI / 2.0f))
                };
            }

            if (gMissilePositions[i].y <= gMissileScale.y / 2.0f)
            {
                gMissilePositions[i].y = gMissileScale.y / 2.0f;
                gMissileTravelAngles[i] = 360.0f - gMissileTravelAngles[i];
                gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;

                if (gMissileTravelAngles[i] >= 360.0f) gMissileTravelAngles[i] -= 360.0f;
                if (gMissileDrawAngles[i] >= 360.0f) gMissileDrawAngles[i] -= 360.0f;

                gMissileMovements[i] = {
                    MISSILE_LAUNCH_SPEED * cos(gMissileTravelAngles[i] * PI / 180.0f),
                    MISSILE_LAUNCH_SPEED * cos((gMissileTravelAngles[i] * PI / 180.0f) - (PI / 2.0f))
                };
            }

            if (isColliding(&missileHeadPosition, &gMissileHeadScale, &reflecterHitboxPosition, &gReflecterHitboxScale)
                && gMissileMovements[i].y > 0.0f)
            {
                float reflecterHitOffset = (missileHeadPosition.x - reflecterHitboxPosition.x) / (gReflecterHitboxScale.x / 2.0f);

                if (reflecterHitOffset < -1.0f) reflecterHitOffset = -1.0f;
                if (reflecterHitOffset > 1.0f) reflecterHitOffset = 1.0f;

                gMissilePositions[i].y = gReflecterPosition.y - (gReflecterScale.y / 2.0f) - (gMissileScale.y / 2.0f);
                gMissileTravelAngles[i] = 270.0f + 20.0f * reflecterHitOffset;
                gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;

                if (gMissileTravelAngles[i] < 0.0f) gMissileTravelAngles[i] += 360.0f;
                if (gMissileTravelAngles[i] >= 360.0f) gMissileTravelAngles[i] -= 360.0f;
                if (gMissileDrawAngles[i] >= 360.0f) gMissileDrawAngles[i] -= 360.0f;

                gMissileMovements[i] = {
                    MISSILE_LAUNCH_SPEED * cos(gMissileTravelAngles[i] * PI / 180.0f),
                    MISSILE_LAUNCH_SPEED * cos((gMissileTravelAngles[i] * PI / 180.0f) - (PI / 2.0f))
                };
            }

            if (isColliding(&missileHeadPosition, &gMissileHeadScale, &boatHitboxPosition, &gBoatHitboxScale))
            {
                for (int j = 0; j < MAX_MISSILES; j++)
                {
                    gMissileIsLaunched[j] = false;
                    gMissileIsVisible[j] = false;
                    gSplashIsVisible[j] = false;
                    gSplashAnimationTimes[j] = 0.0f;
                    gSplashFrames[j] = 0;
                    gMissilePositions[j] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f };
                    gMissileMovements[j] = { 0.0f, 0.0f };
                }

                gBoatIsVisible = false;
                gExplosionIsVisible = true;
                gExplosionAnimationTime = 0.0f;
                gExplosionFrame = 0;
                gExplosionPosition = { gBoatPosition.x, gBoatPosition.y };
                gBombIsVisible = false;
                gBombScale = { 12.0f, 16.0f };
                gBombAngle = 0.0f;
                gWinnerStatus = BOMBER_WINNER;
            }
            else if (gMissilePositions[i].y >= riverTop)
            {
                gMissileIsLaunched[i] = false;
                gMissileIsVisible[i] = false;
                gSplashIsVisible[i] = true;
                gSplashAnimationTimes[i] = 0.0f;
                gSplashFrames[i] = 0;
                gSplashPositions[i] = { gMissilePositions[i].x, riverTop + 8.0f };
            }
        }
    }

    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (gSplashIsVisible[i])
        {
            constexpr float SPLASH_FRAME_TIME = 0.09f;

            gSplashAnimationTimes[i] += deltaTime;
            gSplashFrames[i] = (int) (gSplashAnimationTimes[i] / SPLASH_FRAME_TIME);

                if (gSplashFrames[i] > 3)
                {
                    gSplashIsVisible[i] = false;
                    gSplashFrames[i] = 0;
                    gMissilePositions[i] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f };
                    gMissileTravelAngles[i] = 100.0f + gMissileAimOffset;
                    gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;
                    gMissileMovements[i] = { 0.0f, 0.0f };
                    gMissileIsVisible[i] = false;
                }
            }
        }

    if (gExplosionIsVisible)
    {
        constexpr float EXPLOSION_FRAME_TIME = 0.07f;

        gExplosionAnimationTime += deltaTime;
        gExplosionFrame = (int) (gExplosionAnimationTime / EXPLOSION_FRAME_TIME);

        if (gExplosionFrame > 6)
        {
            gExplosionIsVisible = false;
            gExplosionFrame = 0;
            for (int i = 0; i < MAX_MISSILES; i++)
            {
                gMissileIsVisible[i] = false;
                gMissilePositions[i] = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.12f };
                gMissileTravelAngles[i] = 100.0f + gMissileAimOffset;
                gMissileDrawAngles[i] = gMissileTravelAngles[i] + 140.0f;
                gMissileMovements[i] = { 0.0f, 0.0f };
                gMissileIsLaunched[i] = false;
            }
            gBombIsVisible = true;
            gBombPosition = { gExplosionPosition.x, gExplosionPosition.y };
            gBombScale = { 12.0f, 16.0f };
            gBombAngle = 0.0f;
        }
    }

    if (gBombIsVisible)
    {
        gBombScale.x += 90.0f * deltaTime;
        gBombScale.y += 120.0f * deltaTime;

        if (gBombScale.x > 72.0f) gBombScale.x = 72.0f;
        if (gBombScale.y > 96.0f) gBombScale.y = 96.0f;
    }

    if (gWinnerStatus == NO_WINNER && gMissilesLeft == 0 && gBoatIsVisible)
    {
        bool hasAnyMissileActivity = false;

        for (int i = 0; i < MAX_MISSILES; i++)
        {
            if (gMissileIsLaunched[i]) hasAnyMissileActivity = true;
            if (gSplashIsVisible[i]) hasAnyMissileActivity = true;
        }

        if (!hasAnyMissileActivity && !gExplosionIsVisible && !gBombIsVisible)
        {
            gWinnerStatus = CAPTAIN_WINNER;
            gCaptainWinTextSize = 8.0f;
        }
    }

    if (gWinnerStatus == CAPTAIN_WINNER || gWinnerStatus == BOMBER_WINNER)
    {
        Win_Rotation(deltaTime);
    }

    // Smooth turn animation
    constexpr float BOAT_TURN_SPEED = 300.0f; // degrees per second
    if (gShipStatus == TURNING_AROUND && gWinnerStatus != CAPTAIN_WINNER)
    {
        float angleDiff = gBoatTargetAngle - gBoatAngle;
        if (angleDiff > 180.0f) angleDiff -= 360.0f;
        if (angleDiff < -180.0f) angleDiff += 360.0f;

        float maxStep = BOAT_TURN_SPEED * deltaTime;
        float absoluteAngleDiff = angleDiff;
        if (absoluteAngleDiff < 0.0f) absoluteAngleDiff = -absoluteAngleDiff;

        if (absoluteAngleDiff <= maxStep)
        {
            gBoatAngle = gBoatTargetAngle;
            gShipStatus = gShipStatusAfterTurn;
        }
        else
        {
            gBoatAngle += (angleDiff > 0.0f) ? maxStep : -maxStep;
            if (gBoatAngle >= 360.0f) gBoatAngle -= 360.0f;
            if (gBoatAngle < 0.0f) gBoatAngle += 360.0f;
        }
    }
}

void render()
{
    BeginDrawing();

    ClearBackground((Color){ 150, 215, 255, 255 });

    renderObject(&gSunTexture, &gSunPosition, &gSunScale, gSunAngle);
    renderObject(&gRiverTexture, &gRiverPosition, &gRiverScale, 0.0f);
    renderObject(&gReflecterTexture, &gReflecterPosition, &gReflecterScale, 0.0f);
    DrawText("Missile Left:", 20, 20, 24, WHITE);
    if (gMissilesLeft == 3) DrawText("3", 170, 20, 24, WHITE);
    if (gMissilesLeft == 2) DrawText("2", 170, 20, 24, WHITE);
    if (gMissilesLeft == 1) DrawText("1", 170, 20, 24, WHITE);
    if (gMissilesLeft == 0) DrawText("0", 170, 20, 24, WHITE);
    DrawText("Choose the Number of Missile", 20, 50, 20, WHITE);

    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (gMissileIsVisible[i])
        {
            renderObject(&gMissileTexture, &gMissilePositions[i], &gMissileScale, gMissileDrawAngles[i]);
        }
    }

    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (gSplashIsVisible[i])
        {
            float splashFrameWidth = (float) gSplashTexture.width / 4.0f;
            Rectangle splashTextureArea = {
                splashFrameWidth * gSplashFrames[i],
                0.0f,
                splashFrameWidth,
                (float) gSplashTexture.height
            };

            Rectangle splashDestinationArea = {
                gSplashPositions[i].x,
                gSplashPositions[i].y,
                gSplashScale.x,
                gSplashScale.y
            };

            Vector2 splashOrigin = {
                gSplashScale.x / 2.0f,
                gSplashScale.y / 2.0f
            };

            DrawTexturePro(gSplashTexture, splashTextureArea, splashDestinationArea, splashOrigin, 0.0f, WHITE);
        }
    }

    if (gExplosionIsVisible)
    {
        float explosionFrameWidth = (float) gExplosionTexture.width / 7.0f;
        Rectangle explosionTextureArea = {
            explosionFrameWidth * gExplosionFrame,
            0.0f,
            explosionFrameWidth,
            (float) gExplosionTexture.height
        };

        Rectangle explosionDestinationArea = {
            gExplosionPosition.x,
            gExplosionPosition.y,
            gExplosionScale.x,
            gExplosionScale.y
        };

        Vector2 explosionOrigin = {
            gExplosionScale.x / 2.0f,
            gExplosionScale.y / 2.0f
        };

        DrawTexturePro(gExplosionTexture, explosionTextureArea, explosionDestinationArea, explosionOrigin, 0.0f, WHITE);
    }

    if (gBombIsVisible)
    {
        renderObject(&gMissileTexture, &gBombPosition, &gBombScale, gBombAngle);
        DrawText("bomer win!!!", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 20, 32, WHITE);
    }

    if (gWinnerStatus == CAPTAIN_WINNER)
    {
        DrawText("Captin Win!!!", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 20, (int) gCaptainWinTextSize, WHITE);
    }

    if (gBoatIsVisible)
    {
        Rectangle boatTextureArea = {
            0.0f,
            0.0f,
            (float) gBoatTexture.width,
            (float) gBoatTexture.height
        };

        Rectangle boatDestinationArea = {
            gBoatPosition.x,
            gBoatPosition.y,
            gBoatScale.x,
            gBoatScale.y
        };

        Vector2 boatOrigin = {
            gBoatScale.x / 2.0f,
            gBoatScale.y / 2.0f
        };

        DrawTexturePro(gBoatTexture, boatTextureArea, boatDestinationArea, boatOrigin, gBoatAngle, WHITE);
    }

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gSunTexture);
    UnloadTexture(gRiverTexture);
    UnloadTexture(gBoatTexture);
    UnloadTexture(gMissileTexture);
    UnloadTexture(gSplashTexture);
    UnloadTexture(gExplosionTexture);
    UnloadTexture(gReflecterTexture);
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
