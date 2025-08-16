#ifndef __GAME_PROCESS__
#define __GAME_PROCESS__

#include "game_types.h"
#include "game_common.h"

#define MAX_INTERACTIONS 256
#define DEBUG false

extern ent_t* player;
typedef bool EntFilterFn(ent_t* e, ent_t* other); 
static bool EntNotOnTeam(ent_t* e,ent_t* other){
  if(e->team == other->team || e->team == TEAM_ENVIROMENT)
    return false;
  else
    return true;
}

static bool EntNotOnTeamAlive(ent_t* e,ent_t* other){
  if(e->team == other->team || e->team == TEAM_ENVIROMENT || e->state >= STATE_DIE)
    return false;
  else
    return true;
}

//INTERACTIONS_T===>
typedef struct {
  int             source_uid; //uid of who interacted (body, ent)
  int             target_uid; //uid of interactee (body, ent)
  cooldown_t*     timer;
} interaction_t;

int InitInteractions();
interaction_t* EntInteraction(unsigned int source, unsigned int target, int duration);
bool AddInteraction(interaction_t* inter);
bool CanInteract(int source, int target);
void FreeInteraction(interaction_t* item);
void FreeInteractionByIndex(int i);
void InteractionStep();
//==INTERACTIONS_T==>
//EVENTS==>

typedef enum{
  GAME_LOADING,
  GAME_READY
}GameState;

typedef struct{
  GameState   state;
  events_t    *events;
}game_process_t;
extern game_process_t game_process;

void InitGameProcess();
void GameProcessStep();

//===WORLD_T===>

typedef struct{
  ObjectInstance  ents[ROOM_INSTANCE_COUNT];
  TileInstance    tiles[ROOM_TILE_COUNT];
  unsigned int    num_ents;
}world_data_t;

typedef struct world_s{
  Rectangle     room_bounds;
  bool          intgrid[GRID_WIDTH][GRID_HEIGHT];
  ent_t*        ents[MAX_ENTS];
  unsigned int  num_ent;
  rigid_body_t* cols[MAX_ENTS];
  unsigned int  num_col;
  sprite_t*     sprs[MAX_ENTS];
  unsigned int  num_spr;
} world_t;

int WorldGetEnts(ent_t** results,EntFilterFn fn, void* params);
Vector2 GetWorldCoordsFromIntGrid(Vector2 pos, float len);
bool RegisterEnt( ent_t *e);
bool RegisterRigidBody(rigid_body_t *b);
bool RegisterSprite(sprite_t *s);
void WorldInitOnce();
void WorldPreUpdate();
void WorldFixedUpdate();
void InitWorld(world_data_t data);
void WorldRender();
Rectangle WorldRoomBounds();
#endif

