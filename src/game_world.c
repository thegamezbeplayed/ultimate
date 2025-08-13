#include "game_process.h"

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

Rectangle WorldRoomBounds(){
  return world.room_bounds;
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
    
    SetState(world.ents[i], STATE_IDLE,NULL);
    EntSync(world.ents[i]);
  }

}

void WorldPreUpdate(){
  InteractionStep();
  PhysicsCollision(world.cols,world.num_col,RigidBodyCollide);
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
  world.room_bounds = RecFromCoords(0,0,ROOM_WIDTH,ROOM_HEIGHT);
  for (int i = 0; i < data.num_ents; i++)
    RegisterEnt(InitEnt(data.ents[i]));
  
  for (int j = 0; j < ROOM_TILE_COUNT; j++)
    RegisterEnt(InitEntStatic(data.tiles[j]));
}

void WorldRender(){
  DrawRectangleRec(world.room_bounds, PURPLE);
  
  for(int i = 0; i < world.num_spr;i++){
    DrawSprite(world.sprs[i]);
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
  cooldown_t* loadEvent = InitCooldown(90,GameReady,EVENT_GAME_PROCESS);
  AddEvent(game_process.events,loadEvent);
}

void GameProcessStep(){
  if(game_process.events)
    StepEvents(game_process.events);
}

