#ifndef __GAME_COMMON__
#define __GAME_COMMON__


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
  STATE_ATTACK,
  STATE_AGGRO,
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
//===STATS===>
typedef enum{
  STAT_HEALTH,
  STAT_DAMAGE,
  STAT_SPEED,
  STAT_ACCEL,
  STAT_BLANK//sentinel
}StatType;


#endif
