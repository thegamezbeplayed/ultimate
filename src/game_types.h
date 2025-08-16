#ifndef __GAME_TYPES__
#define __GAME_TYPES__

#include "game_common.h"
#include "game_assets.h"
#include "game_utils.h"
#include "room_data.h"
#include "raylib.h"
#include "game_math.h"

#define GRAVITY 0.65f
#define MAX_VELOCITY  64.0f
#define TERMINAL_VELOCITY 7.0f
#define GRID_SIZE 128
#define GRID_STEP 32

#define MAX_ENTS 64  
#define MAX_ATTACKS 8

typedef struct ent_s ent_t;
typedef struct rigid_body_s rigid_body_t;
typedef struct stat_s stat_t;

typedef bool (*StatCallback)(ent_t* owner);

typedef struct stat_s{
  StatType  attribute;
  float     min;
  float     max;
  float     current;
  float     increment;
  StatCallback on_stat_change;
  StatCallback on_stat_empty;
} stat_t;

stat_t InitStatOnMax(StatType attr, float val);
stat_t InitStatEmpty();
void InitStats(stat_t stats[STAT_BLANK]);
void StatChangeValue(ent_t* owner, stat_t* attr, float val);
void StatMaxOut(stat_t* s);
//<====STATS

//PHYSICS====>
typedef struct rigid_body_s rigid_body_t;

typedef enum {
  //FORCE_GRAVITY,
  FORCE_STEERING,
  FORCE_IMPULSE,
  FORCE_AVOID,
  //FORCE_JUMP,
  FORCE_NONE
}ForceType;

typedef struct bounds_s {
  ShapeType   shape;
  Vector2     pos,offset;
  float       radius;
  float       width,height;
  //BoundsCheckCall col_check;
} bounds_t;

typedef void (*ForceReactionCallback)(rigid_body_t *a, rigid_body_t *b, ForceType type);

typedef struct {
  Vector2    vel;
  ForceType  type;
  Vector2    accel;
  float      max_velocity;
  Vector2    friction;
  float      threshold;
  bool       is_active;
  //int        steps;
  ForceReactionCallback on_react;
}force_t;
//===RIGID_BODY===>
typedef bool (*CollisionCallback)(rigid_body_t* a, rigid_body_t* b, ent_t *e);
void ReactionBumpForce(rigid_body_t* a, rigid_body_t* b, ForceType t);
void ReactionAvoidForce(rigid_body_t* a, rigid_body_t* b, ForceType t);
void CollisionBoundsAvoid(rigid_body_t* a, rigid_body_t* b, ForceType t);
bool CheckStep(rigid_body_t* b, Vector2 vel, float dist, Vector2* out);
bool CheckCanSeeTarget(rigid_body_t* a, rigid_body_t *b, float range);

typedef struct rigid_body_s {
  int           buid; //body uid
  Vector2       velocity;
  Vector2       position;
  force_t       forces[FORCE_NONE];
  bounds_t      collision_bounds;
  int           num_collisions_detected;
  bounds_t      collisions[4];//TODO make define
  ForceType     counter_force[FORCE_NONE];
  float         restitution;
  bool          is_static;
  bool          is_ground;
  bool          is_grounded;
  bool          simulate;
  struct ent_s  *owner;
  int           col_rate;
  CollisionCallback on_collision;
} rigid_body_t;

rigid_body_t* InitRigidBody(ent_t* owner,Vector2 pos, float radius);
rigid_body_t* InitRigidBodyStatic(ent_t* owner,Vector2 pos, float radius);
bool FreeRigidBody(rigid_body_t* b);
//<====RIGID_BODY
static inline float PhysicsSimpleDistCheck(rigid_body_t* a, rigid_body_t* b){
  return Vector2Distance(a->position, b->position);
} 

void PhysicsInitOnce(rigid_body_t* b);
void PhysicsStep(rigid_body_t *b);
void CancelForce(force_t *f);
bool PhysicsStepForce(force_t *force,bool accelerate);
void PhysicsApplyForce(rigid_body_t* body, force_t force);
void PhysicsAccelDir(rigid_body_t *b, ForceType type, Vector2 dir);
void PhysicsSetAccel(rigid_body_t *b, ForceType type,Vector2 accel);
void PhysicsCollision(int i,rigid_body_t* bodies[MAX_ENTS],int num_bodies, CollisionCallback callback);
bool CheckCollision(rigid_body_t *a, rigid_body_t *b, int len);
bool RigidBodyCollide(rigid_body_t* a, rigid_body_t* b, ent_t *e);
force_t ForceFromVector2(ForceType type, Vector2 vec);
force_t ForceEmpty(ForceType type);
force_t ForceBasic(ForceType type);
//<====PHYSICS
typedef struct {
  char          name[MAX_NAME_LEN];
  int           duid;
  int           num_frames;
  int           duration;
  int           reach;
  rigid_body_t  *hurtbox;
  struct ent_s* owner;
}attack_t;

typedef struct{
  ent_t*                target;
  Vector2               destination;
  int                   aggro;
  int                   range;
  bool                  has_arrived;
  behavior_tree_node_t* bt[STATE_END];
}controller_t;
controller_t* InitController(ObjectInstance data);

//===ENT_T===>
typedef struct ent_s{
  int                   uid;
  char*                 name;
  TeamName              team;
  Vector2               pos;
  rigid_body_t          *body;
  EntityState           state;
  controller_t          *control;
  events_t              *events;
  int                   num_attacks;
  int                   active_attack_id;
  attack_t              attacks[MAX_ATTACKS];
  stat_t                stats[STAT_BLANK];
  Vector2               facing;
  sprite_t              *sprite;
} ent_t;

ent_t* InitEntStatic(TileInstance data);
ent_t* InitEnt(ObjectInstance data);
void EntSync(ent_t* e);
void EntKill(ent_t* e);
void EntDestroy(ent_t* e);
void EntFree(ent_t* e);
static inline bool EntTargetable(ent_t* e){
  return (e!=NULL && e->state <= STATE_DIE);
}
void EntControlStep(ent_t *e);
typedef void (*StateChangeCallback)(ent_t *e, EntityState old, EntityState s);
bool CheckEntOutOfBounds(ent_t* e, Rectangle bounds);
bool SetState(ent_t *e, EntityState s,StateChangeCallback callback);
void StepState(ent_t *e);
void OnStateChange(ent_t *e, EntityState old, EntityState s);
bool CanChangeState(EntityState old, EntityState s);

//===ENT_T==>
#endif
