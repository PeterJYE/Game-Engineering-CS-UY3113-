/**

* Author: Peter Ye

* Assignment: Pong Clone

* Date due: 02/14/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/
#include "raylib.h"
#include <math.h>

// Enums
enum AppStatus { TERMINATED, RUNNING };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60;

constexpr char SUN_FP[]   = "assets/textures/sun.png";
constexpr char RIVER_FP[] = "assets/textures/river.png";
constexpr char BOAT_FP[]  = "assets/textures/boat.png";

// Global Variables
AppStatus gAppStatus = RUNNING;

Texture2D gSunTexture;
Texture2D gRiverTexture;
Texture2D gBoatTexture;

float gPreviousTicks = 0.0f;
float gTime          = 0.0f;

Vector2 gSunPosition   = { SCREEN_WIDTH * 0.85f, SCREEN_HEIGHT * 0.16f };
Vector2 gRiverPosition = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.88f };
Vector2 gBoatPosition  = { -55.0f, SCREEN_HEIGHT * 0.78f };

Vector2 gSunScale   = { 100.0f, 100.0f };
Vector2 gRiverScale = { 860.0f, 180.0f };
Vector2 gBoatScale  = { 110.0f, 140.0f };

float gSunAngle = 0.0f;

void initialise();
void processInput();
void update();
void render();
void shutdown();
void renderObject(const Texture2D *texture, const Vector2 *position, const Vector2 *scale, float angle);

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

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battleship");

    gSunTexture   = LoadTexture(SUN_FP);
    gRiverTexture = LoadTexture(RIVER_FP);
    gBoatTexture  = LoadTexture(BOAT_FP);

    SetTargetFPS(FPS);
}

void processInput()
{
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
    gSunPosition = {sunAnchor.x + 25.0f * cos(gTime * 0.9f), sunAnchor.y + 14.0f * sin(gTime * 0.9f)};
    gSunAngle += 55.0f * deltaTime;

    // River
    const Vector2 riverAnchor = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.88f };
    gRiverPosition = {riverAnchor.x + 8.0f * cos(gTime * 0.5f), riverAnchor.y + 6.0f * sin(gTime * 0.8f)};
    gRiverScale = {860.0f + 18.0f * sin(gTime * 1.3f),180.0f + 10.0f * cos(gTime * 1.1f)};

    // Boat
    gBoatPosition.x += 180.0f * deltaTime;
    if (gBoatPosition.x > SCREEN_WIDTH + 55.0f) gBoatPosition.x = -55.0f;

    gBoatPosition.y = gRiverPosition.y - 55.0f + 12.0f * sin(gTime * 2.0f);
    gBoatScale = {
        110.0f + 8.0f * sin(gTime * 1.9f),
        140.0f + 12.0f * cos(gTime * 1.7f)
    };
}

void render()
{
    BeginDrawing();

    ClearBackground((Color){ 150, 215, 255, 255 });

    renderObject(&gSunTexture, &gSunPosition, &gSunScale, gSunAngle);
    renderObject(&gRiverTexture, &gRiverPosition, &gRiverScale, 0.0f);
    renderObject(&gBoatTexture, &gBoatPosition, &gBoatScale, 0.0f);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gSunTexture);
    UnloadTexture(gRiverTexture);
    UnloadTexture(gBoatTexture);
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
