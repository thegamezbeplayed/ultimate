#include "game_process.h"
#include "game_tools.h"

MAKE_ADAPTER(StepState, ent_t*);

void GameReady(){
  WorldInitOnce();
  game_process.state = GAME_READY;
}

static world_t world;
ent_t* player;

int WorldGetEnts(ent_t** results,EntFilterFn fn, void* params){
  int num_results = 0;
  for(int i = 0; i < world.num_ent; i++){
    if(!fn(world.ents[i],params))
      continue;

    results[num_results] = world.ents[i];
    num_results++;
  }

  return num_results;
}

Vector2 GetWorldCoordsFromIntGrid(Vector2 pos, float len){
  int grid_x = (int)pos.x/CELL_WIDTH;
  int grid_y = (int)pos.y/CELL_HEIGHT;

  int grid_step = (int)len/CELL_WIDTH;

  int start_x = CLAMP(grid_x - grid_step,1,GRID_WIDTH);
  int start_y = CLAMP(grid_y - grid_step,1,GRID_HEIGHT);

  int end_x = CLAMP(grid_x + grid_step,1,GRID_WIDTH);
  int end_y = CLAMP(grid_y + grid_step,1,GRID_HEIGHT);

 
 Cell candidates[GRID_WIDTH * GRID_HEIGHT];
 int count = 0;
  for (int x = start_x; x < end_x; x++)
    for(int y = start_y; y < end_y; y++){
      if(world.intgrid[x][y])
        continue;
      if(distance(grid_x,grid_y,x,y) > len)
        continue;

      candidates[count++] = (Cell){x,y};
    }

  if (count == 0)
    return VEC_UNSET;

  int r = rand() % count;

  return CellToVector2(candidates[r],CELL_WIDTH);
}

Rectangle WorldRoomBounds(){
  return world.room_bounds;
}

int RemoveSprite(int index){
  int last_pos = world.num_spr -1;
  
  if(!FreeSprite(world.sprs[index]))
    return 0;

  world.num_spr--;
  if(index!=last_pos){
    world.sprs[index] = world.sprs[last_pos];
    return 1;
  }
 
  return 0;
  
}

int RemoveBody(int index){
  int last_pos = world.num_col -1;
  
  if(!FreeRigidBody(world.cols[index]))
    return 0;

  world.num_col--;
  if(index!=last_pos){
    world.cols[index] = world.cols[last_pos];
    return 1;
  }
 
  return 0;
  
}

int AddEnt(ent_t *e) {
  if (world.num_ent < MAX_ENTS) {
    int index = world.num_ent;
    world.ents[index] = e;
    world.num_ent++;

    return index;
  }
  return -1;
} 

int AddRigidBody(rigid_body_t *b){
  if(world.num_col < MAX_ENTS){
    int index = world.num_col;
    world.cols[world.num_col++] = b;

    return index;
  }

  return -1;
}

int AddSprite(sprite_t *s){
  if(world.num_spr < MAX_ENTS){
    int index = world.num_spr;
    world.sprs[world.num_spr++] = s;

    return index;
  }

  return -1;
}

bool RegisterEnt( ent_t *e){
  e->uid = AddEnt(e);
  if(e->team == TEAM_PLAYER)
    player = e;

  if(e->body)
    RegisterRigidBody(e->body);
  if(e->sprite)
    RegisterSprite(e->sprite);

  return e->uid > -1;
}

bool RegisterRigidBody(rigid_body_t *b){
  b->buid = AddRigidBody(b);

  return b->buid > -1;
}

bool RegisterSprite(sprite_t *s){
  s->suid = AddSprite(s);

  return s->suid > -1;
}

void WorldInitOnce(){
  InteractionStep();

  for(int i = 0; i < world.num_ent; i++){
    if(world.cols[i])
      PhysicsInitOnce(world.cols[i]);
    
    EntSync(world.ents[i]);
   
    cooldown_t* spawner = InitCooldown(3,EVENT_SPAWN,StepState_Adapter,world.ents[i]);
    AddEvent(world.ents[i]->events, spawner);
  }

}

void WorldPreUpdate(){
  InteractionStep();
  for(int i = 0; i < world.num_col; i++){
    if(world.cols[i]->owner!=NULL)
      PhysicsCollision(i,world.cols,world.num_col,RigidBodyCollide);
    else
      i-=RemoveBody(i);
  }
}

void WorldFixedUpdate(){
  for(int i = 0; i < world.num_ent; i++){
    switch(world.ents[i]->state){
      case STATE_END:
        break;
      case STATE_DIE:
        EntDestroy(world.ents[i]);
        break;
      default:
        PhysicsStep(world.ents[i]->body);
        EntSync(world.ents[i]);
        if(!CheckEntOutOfBounds(world.ents[i], WorldRoomBounds()))
          EntKill(world.ents[i]);
        break;
    }
  }
}

void InitWorld(world_data_t data){
  world = (world_t){0};
  for(int x = 0; x < GRID_WIDTH; x++)
    for(int y = 0; y < GRID_HEIGHT; y++)
      world.intgrid[x][y] = false;

  world.room_bounds = RecFromCoords(0,0,ROOM_WIDTH,ROOM_HEIGHT);
  for (int i = 0; i < data.num_ents; i++)
    RegisterEnt(InitEnt(data.ents[i]));
  
  for (int j = 0; j < ROOM_TILE_COUNT; j++){
    world.intgrid[data.tiles[j].map_x][data.tiles[j].map_y] = true;
    RegisterEnt(InitEntStatic(data.tiles[j]));
  }
}

void WorldRender(){
  DrawRectangleRec(world.room_bounds, PURPLE);
  
  for(int i = 0; i < world.num_spr;i++){
    if(world.sprs[i]->owner !=NULL)
      DrawSprite(world.sprs[i]);
    else
      i-=RemoveSprite(i);
    
    if(!DEBUG)
      continue;
    Rectangle bounds = {
      .x = world.cols[i]->collision_bounds.pos.x,
      .y = world.cols[i]->collision_bounds.pos.y,
      .width = world.cols[i]->collision_bounds.width,
      .height = world.cols[i]->collision_bounds.height
    };
    DrawRectangleRec(bounds, BLUE);
  }
}

void InitGameProcess(){
  game_process.state = GAME_LOADING;
  game_process.events = InitEvents();
  cooldown_t* loadEvent = InitCooldown(90,EVENT_GAME_PROCESS,GameReady,NULL);
  AddEvent(game_process.events,loadEvent);
}

void GameProcessStep(){
  if(game_process.events)
    StepEvents(game_process.events);
}

