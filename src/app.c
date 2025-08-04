#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
static const int screenWidth = 1600;
static const int screenHeight = 900;

static void UpdateDrawFrame(void);          // Update and draw one frame

int main(void)
{
    // Initialization
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib game template");

    InitAudioDevice();      // Initialize audio device
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    //--------------------------------------------------------------------------------------

    InitGameplayScreen();
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif
    // Unload global data loaded
    //UnloadFont(font);
  //  UnloadMusicStream(music);
//    UnloadSound(fxCoin);

    CloseAudioDevice();     // Close audio context

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

static void UpdateDrawFrame(void)
{
  UpdateGameplayScreen();
  BeginDrawing();

  DrawGameplayScreen();
  DrawFPS(10, 10);

  EndDrawing();

}


