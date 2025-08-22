#ifndef __GAME_COMMON__
#define __GAME_COMMON__

#include "raylib.h"

#define MAX_EVENTS 16

//====EVENTS===>
typedef enum{
  EVENT_GAME_PROCESS,
  EVENT_INTERACTION,
  EVENT_SPAWN,
  EVENT_ATTACK_INPUT,
  EVENT_ATTACK_RATE,
  EVENT_PLAY_SFX,
  EVENT_NONE
} EventType;

typedef void (*CooldownCallback)(void* params);

typedef struct{
  EventType         type;
  int               duration;
  int               elapsed;
  bool              is_complete;
  void*             on_end_params;
  CooldownCallback  on_end;
}cooldown_t;
cooldown_t* InitCooldown(int dur, EventType type, CooldownCallback on_end_callback, void* params);
void UnloadCooldown(cooldown_t* cd);
typedef struct{
  cooldown_t  cooldowns[MAX_EVENTS];
  bool        cooldown_used[MAX_EVENTS];
}events_t;


events_t* InitEvents();
void UnloadEvents(events_t* ev);
bool AddEvent(events_t* pool, cooldown_t* cd);
void StepEvents(events_t* pool);
bool CheckEvent(events_t* pool, EventType);
//<======EVENTS>
typedef enum{
  TEAM_PLAYER,
  TEAM_ENEMIES,
  TEAM_ENVIROMENT,
  TEAM_NONE
}TeamName;

typedef enum EntityStateEnum {
  SHAPE_CIRCLE,
  SHAPE_RECTANGLE,
  SHAPE_TRIANGLE,
  SHAPE_NONE
} ShapeType;

typedef enum{
  STATE_NONE,//if ent_t is properly initalized to {0} this is already set
  STATE_SPAWN,//Should only be set after NONE
  STATE_IDLE, //should be able to move freely between these ==>
  STATE_WANDER,
  STATE_AGGRO,
  STATE_ATTACK,
  STATE_DIE,//<===== In MOST cases. Should not be able to go down from DIE
  STATE_END//sentinel entity state should never be this or greater
}EntityState;

typedef struct {
  const char* name;
  EntityState   state;
}EntityStateAlias;

static EntityStateAlias ent_state_alias[STATE_END] = {
  {"STATE_IDLE",    STATE_IDLE},
  {"STATE_WANDER",  STATE_WANDER},
  {"STATE_SPAWN",   STATE_SPAWN},
  {"STATE_ATTACK",  STATE_ATTACK},
  {"STATE_AGGRO",   STATE_AGGRO},
  {"STATE_DIE",     STATE_DIE},
  {"STATE_END",     STATE_END} 
};

EntityState EntityStateLookup(const char* name);
const char* EntityStateName(EntityState s);
//===STATS===>
typedef enum{
  STAT_HEALTH,
  STAT_DAMAGE,
  STAT_SPEED,
  STAT_ACCEL,
  STAT_BLANK//sentinel
}StatType;

#endif
