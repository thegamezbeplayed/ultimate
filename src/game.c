#include "raylib.h"
#include "game_process.h"
#include "screens.h"    // NOTE: Declares global (extern) variables and screens functions

Camera2D camera = { 0 };
Texture2D sprite_sheet;
game_process_t game_process;
JNode* raw_game_data;
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
double currentTime = 0.0;           // Current time measure
double updateDrawTime = 0.0;        // Update + Draw time
double previousTime = 0.0;    // Previous time measure
double waitTime = 0.0;              // Wait time (if target fps required)
float deltaTime = 0.0f;             // Frame time (Update + Draw + Wait time)

int fixedFPS = 60;

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
void InitGameplayScreen(void)
{
  InitGameProcess();

  //camera.target = player.position;
  camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  
  Image spritesImg = LoadImage(TextFormat("resources/%s",ATLAS_RAY_SHEET_IMAGE_PATH));
  sprite_sheet = LoadTextureFromImage(spritesImg);

  struct json_object* rawJNode = NULL;

  LoadJson("resources/bt.json",&rawJNode);

  if(rawJNode!=NULL){
    raw_game_data = ParseJNode(rawJNode);
  }
  
  world_data_t wdata = {0};
  for (int i = 0; i < ROOM_INSTANCE_COUNT; i++){
    if(room_instances[i].team_enum > -1)
      wdata.ents[wdata.num_ents++] = room_instances[i];
  }

  for (int j = 0; j < ROOM_TILE_COUNT; j++){
    wdata.tiles[j] = room_tiles[j];
  }
  
  InitWorld(wdata);
}

void PreUpdate(void){
  if(game_process.state == GAME_LOADING)
    return;

  WorldPreUpdate();
}

void FixedUpdate(void){
  if(game_process.state == GAME_LOADING)
    return;

  Vector2 input = CaptureInput();

  if(Vector2Length(input)>0){
    input = Vector2Scale(input,8);//TODO this is placeholder for ent speed
    PhysicsApplyForce(player->body,ForceFromVector2(FORCE_STEERING,input));
  }
  WorldFixedUpdate();
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
      GameProcessStep();
      //PostUpdate();
      waitTime = 0;
    }
  }
  else
    deltaTime = (float)updateDrawTime;    // Framerate could be variable

  previousTime = currentTime;
  
  if(player)
    camera.target = player->pos;
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
  BeginMode2D(camera);

  // TODO: Draw GAMEPLAY screen here!
  DrawRectangle(0, 0, 4 * GetScreenWidth(), 4 * GetScreenHeight(), PURPLE);
  Draw2DGrid(GRID_SIZE, GetScreenWidth()*4, GetScreenHeight()*4);
  WorldRender();

  EndMode2D();

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
