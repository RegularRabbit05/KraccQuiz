#include <time.h>
#include <stdio.h>
#include "raylib.h"
#include "resources/loader.c"

struct VideoPlayer {
    struct ImageLoader frames;
    uint32_t frame;
    double lastFrameTime;
};

struct Sprite {
    int pos[2];
};

#define sizeX 600
#define sizeY 1000

#define textSize 40
const char *text = "Who's the real Mr Best?";

void nullFunc(__attribute__((unused)) int a, __attribute__((unused)) const char* b, __attribute__((unused)) va_list c) {}

void updateTitle() {
    SetWindowTitle(TextFormat("Kracc Quizz | FPS: %d | FT: %0.2f", GetFPS(), GetFrameTime() * 1000));
}

int main() {
    bool promptDone = false;
    bool clickedFirst = false;
    bool firstDoneLaugh = false;
    struct Sprite laughEmojis[512];
    bool clickedSecond = false;
    bool rockStart = false;

    SetTraceLogCallback(nullFunc);
    SetRandomSeed(time(0));
    for (int i = 0; i < sizeof laughEmojis / sizeof (struct Sprite); i++) {
        laughEmojis[i].pos[0] = GetRandomValue(-40, sizeX);
        laughEmojis[i].pos[1] = GetRandomValue(-1512, 5);
    }
    InitAudioDevice();
    Sound soundTrack = loadSound(getSongData());
    SetSoundVolume(soundTrack, 0.1f);
    Sound promptS = loadSound(getPromptData());
    Sound effectAS = loadSound(getEffectAData());
    Sound wrongS = loadSound(getWrongData());
    Sound laughS = loadSound(getLaughData());
    Sound rockS = loadSound(getRockSData());
    Sound failS = loadSound(getFailData());
    Sound deadS = loadSound(getDeadSData());
    SetSoundVolume(wrongS, 0.5f);
    SetSoundVolume(effectAS, 1.5f);
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(sizeX, sizeY, "");
    HideCursor();
    Texture curTex;
    {
        Image cur = loadImage(getCursorData());
        curTex = LoadTextureFromImage(cur);
        UnloadImage(cur);
    }
    {
        Image icon = loadImage(getIconData());
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    SetExitKey(2);

    struct VideoPlayer mrBeast = {loadAllFrames(getMrBeastData()), 0, 0};
    struct VideoPlayer theRock = {loadAllFrames(getTheRockData()), 0, 0};
    struct VideoPlayer theDream = {loadAllFrames(getDreamData()), 0, 0};

    int textX = MeasureText(text, textSize);

    Texture crossTex = loadTexture(getCrossData());
    Texture laughTex = loadTexture(getLaughingData());
    Texture pointTex = loadTexture(getPointData());

    PlaySound(soundTrack);
    PlaySound(promptS);

    mrBeast.lastFrameTime = GetTime();
    double lastTime = GetTime();
    bool seq = false;
    while (!WindowShouldClose() && !seq) {
        updateTitle();
        Texture tex1 = LoadTextureFromImage(mrBeast.frames.buffer[mrBeast.frame]);
        Texture tex2 = LoadTextureFromImage(theRock.frames.buffer[(theRock.frame != -1) ? theRock.frame : 0]);
        Texture tex3 = LoadTextureFromImage(theDream.frames.buffer[theDream.frame]);

        Vector2 pos = GetMousePosition();
        if (!clickedFirst && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && promptDone && !IsSoundPlaying(effectAS)) {
            if (CheckCollisionPointRec(pos, (Rectangle) {(float) sizeX / 6, (float) sizeY / 3, 128 * 1.5f, 228 * 1.5f})) {
                clickedFirst = true;
                PlaySound(wrongS);
            }
        }

        if (theRock.frame == -1 && !clickedSecond && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clickedSecond = true;
            StopSound(soundTrack);
            PlaySound(failS);
        }

        BeginDrawing();
        ClearBackground((Color){ 0, 186, 227, 255});
        DrawText(text, sizeX / 2 - textX / 2, 200, textSize, BLACK);

        DrawTextureEx(tex1, (Vector2) {(float) sizeX / 6, (float) sizeY / 3}, 0, 1.5f, WHITE);
        if (clickedFirst) DrawTexture(crossTex, sizeX / 6, sizeY / 3 + 20, WHITE);
        else DrawTexture(pointTex, -20, 620, WHITE);

        if (clickedFirst && !firstDoneLaugh) {
            if (!IsSoundPlaying(laughS)) {
                PlaySound(laughS);
            }
            firstDoneLaugh = true;
            for (int i = 0; i < sizeof laughEmojis / sizeof (struct Sprite); i++) {
                laughEmojis[i].pos[1] += (int) ((GetTime() - lastTime) * 0.5);
                DrawTexture(laughTex, laughEmojis[i].pos[0], laughEmojis[i].pos[1], WHITE);
                if (laughEmojis[i].pos[1] < sizeY + 50) firstDoneLaugh = false;
            }
        }

        if (clickedFirst && (!IsSoundPlaying(laughS) || rockStart) && theRock.frame != -1) {
            rockStart = true;
            DrawTextureEx(tex2, (Vector2) {150, 580}, 0, 2.0f, WHITE);
            if (theRock.frame == 0 || theRock.lastFrameTime * 1000 + 1000.0f / (float) theRock.frames.fps <= GetTime() * 1000) {
                if (theRock.frame == 0) PlaySound(rockS);
                theRock.lastFrameTime = GetTime();
                theRock.frame++;
                if (theRock.frame >= theRock.frames.oSize) {
                    theRock.frame = -1;
                }
            }
        }

        if (theRock.frame == -1) {
            DrawTextureEx(tex3, (Vector2) {(float) sizeX / 6 * 3, (float) sizeY / 3}, 0, 1.5f, WHITE);
            if (theDream.frame == 0 || theDream.lastFrameTime * 1000 + 1000.0f / (float) theDream.frames.fps <= GetTime() * 1000) {
                theDream.lastFrameTime = GetTime();
                theDream.frame++;
                if (theDream.frame >= theDream.frames.oSize) {
                    theDream.frame = 0;
                }
            }
        }

        if (!IsSoundPlaying(failS) && clickedSecond) {
            seq = true;
        }

        if (IsCursorOnScreen()) DrawTexture(curTex, GetMouseX(), GetMouseY(), WHITE);
        EndDrawing();
        UnloadTexture(tex1);
        UnloadTexture(tex2);
        UnloadTexture(tex3);

        if (mrBeast.lastFrameTime * 1000 + 1000.0f / (float) mrBeast.frames.fps <= GetTime() * 1000) {
            mrBeast.frame++;
            if (mrBeast.frame >= mrBeast.frames.oSize) {
                mrBeast.frame = 0;
            }
            mrBeast.lastFrameTime = GetTime();
        }

        if (!IsSoundPlaying(soundTrack) && !clickedSecond) PlaySound(soundTrack);
        if (!IsSoundPlaying(promptS) && !promptDone) {
            PlaySound(effectAS);
            promptDone = true;
        }
    }

    unloadAllFrames(mrBeast.frames);
    unloadAllFrames(theRock.frames);
    unloadAllFrames(theDream.frames);

    struct VideoPlayer deadV = {loadAllFrames(getDeadData()), 0, GetTime()};
    PlaySound(deadS);
    while (!WindowShouldClose() && seq) {
        updateTitle();
        Texture tex = LoadTextureFromImage(deadV.frames.buffer[deadV.frame]);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
        if (deadV.lastFrameTime * 1000 + 1000.0f / (float) deadV.frames.fps <= GetTime() * 1000) {
            deadV.frame++;
            if (deadV.frame >= deadV.frames.oSize) {
                seq = false;
                deadV.frame = 0;
            }
            deadV.lastFrameTime = GetTime();
        }
        UnloadTexture(tex);
    }

    unloadAllFrames(deadV.frames);

    UnloadTexture(curTex);
    UnloadTexture(crossTex);
    UnloadTexture(laughTex);
    UnloadTexture(pointTex);

    CloseWindow();

    StopSound(soundTrack);
    UnloadSound(soundTrack);
    UnloadSound(promptS);
    UnloadSound(effectAS);
    UnloadSound(wrongS);
    UnloadSound(laughS);
    UnloadSound(rockS);
    UnloadSound(failS);
    UnloadSound(deadS);

    CloseAudioDevice();
    return 0;
}
