#include "raylib.h"
#include "game_process.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

Camera2D camera = { 0 };
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void Draw2DGrid(int cellSize, int screenWidth, int screenHeight) {
  Color gridColor = GRAY;

  // Vertical lines
  for (int x = 0; x <= screenWidth; x += cellSize) {
    DrawLine(x, 0, x, screenHeight, gridColor);
  }

  // Horizontal lines
  for (int y = 0; y <= screenHeight; y += cellSize) {
    DrawLine(0, y, screenWidth, y, gridColor);
  }
}

Vector2 CaptureInput(){
  Vector2 input = {0.0f,0.0f};

  if (IsKeyDown(KEY_D)) input.x += 1.0f;
  if (IsKeyDown(KEY_A)) input.x -= 1.0f;
  if (IsKeyDown(KEY_W)) input.y -= 1.0f;
  if (IsKeyDown(KEY_S)) input.y += 1.0f;

  return input;
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void){
  //camera.target = player.position;
  camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
 
  camera.target = (Vector2){ROOM_WIDTH/2,ROOM_HEIGHT/2};
  InitGameEvents();
}

void PreUpdate(void){
  AudioStep();
  WorldPreUpdate();
}

void FixedUpdate(void){

  if(player && player->state < STATE_DIE){
    Vector2 input = CaptureInput();

    if(Vector2Length(input)>0){
      input = Vector2Scale(input,8);//TODO this is placeholder for ent speed
      PhysicsApplyForce(player->body,ForceFromVector2(FORCE_STEERING,input));
    }
  }
  WorldFixedUpdate();
}
// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
 /* 
  if(player)
    camera.target = player->pos;
    */
  GameProcessStep();
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
  //  if(game_process.state == GAME_LOADING)
  //  return;
  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode2D(camera);

  Draw2DGrid(GRID_SIZE, GetScreenWidth()*4, GetScreenHeight()*4);
  WorldRender();

  EndMode2D();
  DrawFPS(10, 10);

  EndDrawing();

}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
  GameProcessEnd();
  // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
//  return finishScreen;
}
