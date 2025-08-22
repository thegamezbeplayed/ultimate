#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions
#include "game_assets.h"
#include "game_process.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
static const int screenWidth = 1920;
static const int screenHeight = 1080;

double currentTime = 0.0;           // Current time measure
double updateDrawTime = 0.0;        // Update + Draw time
double previousTime = 0.0;    // Previous time measure
double waitTime = 0.0;              // Wait time (if target fps required)
float deltaTime = 0.0f;             // Frame time (Update + Draw + Wait time)

int fixedFPS = 60;

static void UpdateDrawFrame(void);          // Update and draw one frame
static void ChangeToScreen(GameScreen screen);     // Change to screen, no transition effect

static void TransitionToScreen(GameScreen screen); // Request transition to next screen
static void UpdateTransition(void);         // Update transition effect
static void DrawTransition(void);           // Draw transition effect (full-screen rectangle)

int main(void)
{
  // Initialization
  //---------------------------------------------------------
  InitWindow(screenWidth, screenHeight, "raylib game template");

  InitAudioDevice();      // Initialize audio device
                          //--------------------------------------------------------------------------------------


  InitAudio();
  InitResources();

  InitGameProcess();
  //    InitGameplayScreen();
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else

  SetExitKey(KEY_NULL);
  // Main game loop
  while (!WindowShouldClose() && !wantQuit)    // Detect window close button or ESC key
  {
    bool wait = true;
    if (IsKeyPressed(KEY_ENTER))// || IsGestureDetected(GESTURE_TAP))
    {
      GameTransitionScreen();
    }

    currentTime = GetTime();
    updateDrawTime = currentTime - previousTime;

    if(fixedFPS>0){
      waitTime += updateDrawTime;
      if (waitTime < 1.0f/fixedFPS)
      {
        //WaitTime((float)waitTime);
        currentTime = GetTime();
        deltaTime = (float)(currentTime - previousTime);
      }
      else{
        wait = false;        
        waitTime = 0;
      }
    }
    else
      deltaTime = (float)updateDrawTime;    // Framerate could be variable

    previousTime = currentTime;
    GameProcessSync(wait);
  }
#endif
  // Unload global data loaded
  //UnloadFont(font);
  //  UnloadMusicStream(music);

  CloseAudioDevice();     // Close audio context

  CloseWindow();          // Close window and OpenGL context
                          //--------------------------------------------------------------------------------------

  return 0;
}
