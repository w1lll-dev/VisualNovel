#include "dialogue.hpp"
#include <raylib.h>

constexpr unsigned short winWidth = 1600;
constexpr unsigned short winHeight = 900;

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(winWidth, winHeight, "Rizz Simulator");

    SetTargetFPS(60);

    DialogueManager diaMgr;

    while (!WindowShouldClose()) {
        diaMgr.ManageDialogue();

        BeginDrawing();

        diaMgr.DrawBg();
        diaMgr.DrawCharacter();
        diaMgr.DrawDialogue();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}