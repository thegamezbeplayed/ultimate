#include "raylib.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

Camera2D camera = { 0 };

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
double currentTime = 0.0;           // Current time measure
double updateDrawTime = 0.0;        // Update + Draw time
double previousTime = 0.0;    // Previous time measure
double waitTime = 0.0;              // Wait time (if target fps required)
float deltaTime = 0.0f;             // Frame time (Update + Draw + Wait time)

int fixedFPS = 60;



// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
  //camera.target = player.position;
  camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  
}

void PreUpdate(void){
}

void FixedUpdate(void){
}
// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
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
      PreUpdate();
      FixedUpdate();
//      GameProcessStep();
      //PostUpdate();
      waitTime = 0;
    }
  }
  else
    deltaTime = (float)updateDrawTime;    // Framerate could be variable

  previousTime = currentTime;

  // Press enter or tap to change to ENDING screen
  if (IsKeyPressed(KEY_ESCAPE))
  {
    //finishScreen = 1;
  }
 
//  camera.target = player->pos;
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
//  BeginMode2D(camera);

  // TODO: Draw GAMEPLAY screen here!
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), PURPLE);
  

  //EndMode2D();

}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
  // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
//  return finishScreen;
}
