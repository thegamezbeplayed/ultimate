#include "game_types.h"
#include "game_process.h"
#include "raymath.h"

static inline Rectangle RecFromBounds(bounds_t b){
  Rectangle result = {
    .x =      b.pos.x,
    .y =      b.pos.y,
    .width =  b.width,
    .height = b.height
  };

  return result;
}

rigid_body_t* InitRigidBody(ent_t* owner, Vector2 pos, float radius){
  rigid_body_t* b = malloc(sizeof(rigid_body_t));

  *b = (rigid_body_t){0};
  b->owner = owner;
  b->position = pos;
  b->velocity = Vector2Zero();

  b->forces[FORCE_AVOID] = ForceBasic(FORCE_AVOID);
  b->forces[FORCE_STEERING] = ForceBasic(FORCE_STEERING);
  b->forces[FORCE_STEERING].max_velocity = GRID_SIZE/8;

  b->forces[FORCE_IMPULSE] = ForceBasic(FORCE_IMPULSE);
  b->forces[FORCE_IMPULSE].on_react = ReactionBumpForce;
  b->forces[FORCE_IMPULSE].threshold = 1.5F;
  b->forces[FORCE_IMPULSE].friction = Vector2FromXY(0.925,0.925);
  b->counter_force[FORCE_STEERING] = FORCE_IMPULSE;
  b->counter_force[FORCE_IMPULSE] = FORCE_NONE;
  b->counter_force[FORCE_NONE] = FORCE_NONE;
  b->restitution = .5;  
  b->is_static = false;
  b->simulate = false;
  b->col_rate = 4;
  //b->on_collision = NoOpCollision;
  b->collision_bounds = (bounds_t){
    .shape = SHAPE_RECTANGLE,
      .pos = Vector2Zero(),
      .offset = Vector2FromXY(-radius,-radius),
      .radius = radius,
      .width = radius*2,
      .height = radius*2
  };

  return b;
}

rigid_body_t* InitRigidBodyStatic(ent_t* owner, Vector2 pos,float radius){
  rigid_body_t* b = malloc(sizeof(rigid_body_t));

  *b = (rigid_body_t){0};
  b->owner = owner;
  b->position = pos;
  b->velocity = Vector2Zero();

  b->forces[FORCE_IMPULSE] = ForceBasic(FORCE_IMPULSE);
  b->forces[FORCE_IMPULSE].on_react = ReactionBumpForce;
  b->forces[FORCE_IMPULSE].threshold = 0.9F;
  b->forces[FORCE_IMPULSE].friction = Vector2FromXY(0.965,0.965);

  b->forces[FORCE_AVOID] = ForceBasic(FORCE_AVOID);
  b->forces[FORCE_AVOID].on_react = CollisionBoundsAvoid;
  
  b->forces[FORCE_STEERING] = ForceBasic(FORCE_STEERING);
  b->counter_force[FORCE_STEERING] = FORCE_IMPULSE;
  b->counter_force[FORCE_IMPULSE] = FORCE_NONE;
  b->counter_force[FORCE_NONE] = FORCE_NONE;

  b->restitution = 0.925;
  b->is_static = true;
  b->simulate = false;
  b->col_rate = 1;
  //b->on_collision = NoOpCollision;
  b->collision_bounds = (bounds_t){
    .shape = SHAPE_RECTANGLE,
      .pos = Vector2Zero(),
      .offset = Vector2FromXY(-radius,-radius),
      .radius = radius,
      .width = radius*2,
      .height = radius*2
  };
  return b;
}

bool FreeRigidBody(rigid_body_t* b){
  if(!b)
    return false;

  free(b);
  return true;
}

void PhysicsInitOnce(rigid_body_t* b){
  b->collision_bounds.pos = Vector2Add(b->collision_bounds.offset,b->position);
  b->owner->pos = b->position;
  b->simulate = true;
}

void PhysicsStep(rigid_body_t *b){
  if(!b->simulate)
    return;

  b->velocity = Vector2Zero();

  for (int i = 0; i < FORCE_NONE; i++){
    if(b->forces[i].type == FORCE_NONE)
      continue;

    if(b->forces[i].is_active &&
        b->counter_force[i] != FORCE_NONE &&
        b->forces[b->counter_force[i]].is_active)
      CancelForce(&b->forces[i]);

    b->forces[i].is_active = PhysicsStepForce(&b->forces[i],b->forces[i].is_active);
    if(b->forces[i].is_active){
      b->velocity = Vector2Add(b->velocity,b->forces[i].vel);
    }
  }
  int steps = ceilf(Vector2Length(b->velocity)/GRID_STEP);

  Vector2 velStep = Vector2Lerp(Vector2Zero(),b->velocity,1/steps);
  for(int j = 0; j<steps; j++){
    Vector2 applyStep = velStep;
    if(CheckStep(b,velStep,GRID_STEP,&applyStep))
        b->position = Vector2Add(b->position, applyStep);
    else
      break;
  }

  b->num_collisions_detected = 0;
  if(b->owner==NULL)
    return;

  b->collision_bounds.pos = Vector2Add(b->collision_bounds.offset,b->position);
  b->owner->pos = b->position;

}

bool PhysicsStepForce(force_t *force,bool accelerate){
  if(accelerate){
    force->vel = Vector2Add(force->vel,force->accel);
    force->vel = Vector2Lerp(force->vel, Vector2Zero(), 1-force->friction.x);
  }
  else{
    force->vel = Vector2Multiply(force->vel,force->friction);

    if(Vector2Length(force->vel) < force->threshold)
      force->vel = Vector2Zero();
  }

  force->accel = Vector2Zero();
  force->vel = Vector2ClampValue(force->vel,0,force->max_velocity);

  return Vector2Length(force->vel) > force->threshold;
}

bool CheckStep(rigid_body_t* b, Vector2 vel, float dist, Vector2* out){

  if(b->num_collisions_detected == 0)
    return true;  
  else
    TraceLog(LOG_INFO,"Checking collision steps for %d",b->buid);

  if(b->buid !=1)
    DO_NOTHING();
  bool trimmed = false;

  Vector2 testStep = Vector2Add(b->position,vel);

  Rectangle testRec = RecFromCoords(testStep.x+b->collision_bounds.offset.x,
      testStep.y+b->collision_bounds.offset.y,
      b->collision_bounds.width, b->collision_bounds.height);

  for (int i = 0; i<b->num_collisions_detected;i++){
    Vector2 distCheck = VectorDistanceBetween(testStep,b->collisions[i].pos);
    if(Vector2Length(distCheck) < dist)
      continue;

    Rectangle otherBoundsRec = RecFromBounds(b->collisions[i]);
    Rectangle overlap;
    Vector2 normal = GetNormalFromRecs(otherBoundsRec,testRec,&overlap);

    float xCorrection = 0.0f;
    if(normal.x!= 0)
      xCorrection = normal.x*overlap.width;

    testRec.x += xCorrection;
    testStep.x += xCorrection;

    float yCorrection = 0.0f;
    if(normal.y!= 0)
      yCorrection = normal.y*overlap.height;

    testRec.y += yCorrection;
    testStep.y += yCorrection;
  }

  *out = Vector2Subtract(testStep,b->position);

  if(trimmed)
    TraceLog(LOG_INFO,"Velocity <%0.2f,%0.2f> trimmed to <%0.2f,%0.2f>",vel.x,vel.y,out->x,out->y);

  return true;
}

void PhysicsApplyForce(rigid_body_t* body, force_t force){
  if(body->forces[force.type].type == FORCE_NONE)
    return;

  body->forces[force.type].accel = force.accel;
  body->forces[force.type].is_active = true;
}

void PhysicsAccelDir(rigid_body_t *b, ForceType type, Vector2 dir){
  Vector2 accel = Vector2Scale(dir,b->owner->stats[STAT_ACCEL].current);

  PhysicsSetAccel(b,type,accel);
}

void PhysicsSetAccel(rigid_body_t *b, ForceType type,Vector2 accel){
  b->forces[type].accel = accel;
  b->forces[type].is_active = Vector2Length(accel) > 0;
}

void CancelForce(force_t *f){
  f->accel = Vector2Zero();
  f->vel = Vector2Zero();
  f->is_active = false;
}

force_t ForceFromVector2(ForceType type, Vector2 vec){
  force_t g;;
  g.type = type;
  g.vel = Vector2Zero();
  g.accel = vec;
  g.max_velocity = MAX_VELOCITY;
  g.friction = Vector2One();
  g.threshold = 0.0f;
  g.is_active = true;
  return g;

}

force_t ForceBasic(ForceType type){
  force_t g;//= malloc(sizeof(force_t));
  memset(&g, 0, sizeof(force_t));  // Set all bytes to 0
  
  g.type = type;
  g.vel = Vector2Zero();
  g.accel = Vector2Zero();
  g.max_velocity = MAX_VELOCITY;
  g.friction = (Vector2){0.9,0.9};
  g.threshold = 0.825f;
  g.is_active = false;
  return g;
}

force_t ForceEmpty(ForceType type){
  force_t g;//= malloc(sizeof(force_t));
  g.type = type;
  g.vel = Vector2Zero();
  g.accel = Vector2Zero();
  g.max_velocity = MAX_VELOCITY;
  g.friction = Vector2Zero();
  g.threshold = 0.0f;
  g.is_active = false;
  return g;
}

void PhysicsCollision(int i, rigid_body_t* bodies[MAX_ENTS],int num_bodies, CollisionCallback callback){
  if(!bodies[i]->simulate)
    return;

  for (int j = 0; j < num_bodies; j++){
    if(i == j)
      continue;

    if(bodies[i]->owner == bodies[j]->owner)
      continue;

    if(!bodies[j]->simulate)
      continue;

    if(bodies[j]->is_static)
      continue;

    if(!CanInteract(bodies[i]->buid, bodies[j]->buid))
      continue;

    if(!CheckCollision(bodies[i], bodies[j],0))
      continue;

    if(callback)
      if(callback(bodies[i],bodies[j],bodies[j]->owner)){
        AddInteraction(EntInteraction(i,j,bodies[i]->col_rate));
        AudioPlayRandomSfx(SFX_ACTION);
      }
  }
}

bool CheckCollision(rigid_body_t *a, rigid_body_t *b, int len) {
  bool col = false;
  switch(COMBO_KEY(a->collision_bounds.shape, b->collision_bounds.shape)){
    case COMBO_KEY(SHAPE_CIRCLE, SHAPE_CIRCLE):
      col = CheckCollisionCircles(a->collision_bounds.pos,a->collision_bounds.radius, b->collision_bounds.pos, b->collision_bounds.radius+len);
      break;
    case COMBO_KEY(SHAPE_RECTANGLE, SHAPE_CIRCLE):
      col = CheckCollisionCircleRec(b->collision_bounds.pos, b->collision_bounds.radius, (Rectangle){
          .x =      a->collision_bounds.pos.x,
          .y =      a->collision_bounds.pos.y,
          .width =  a->collision_bounds.width,
          .height = a->collision_bounds.height
          });
      break;
    case COMBO_KEY(SHAPE_CIRCLE,SHAPE_RECTANGLE):
      col = CheckCollisionCircleRec(a->collision_bounds.pos, a->collision_bounds.radius, (Rectangle){
          .x =      b->collision_bounds.pos.x,
          .y =      b->collision_bounds.pos.y,
          .width =  b->collision_bounds.width,
          .height = b->collision_bounds.height
          });
      break;
    case COMBO_KEY(SHAPE_RECTANGLE,SHAPE_RECTANGLE):
      Rectangle recA = RecFromCoords(a->collision_bounds.pos.x,a->collision_bounds.pos.y,a->collision_bounds.width,a->collision_bounds.height);
      
      Rectangle recB = RecFromCoords(b->collision_bounds.pos.x,b->collision_bounds.pos.y,b->collision_bounds.width,b->collision_bounds.height);
      
      col = CheckCollisionRecs(recA,recB);
      break;
  }

  return col;
}

bool RigidBodyCollide(rigid_body_t* a, rigid_body_t* b, ent_t *e){
  if(b->is_static)
    return false;

  for (int i = 0; i < FORCE_NONE; i++){
    if (a->forces[i].type == FORCE_NONE || !a->forces[i].on_react)
      continue;

    a->forces[i].on_react(b,a,i);
  }
    
  return true;
}

void CollisionBoundsAvoid(rigid_body_t* a, rigid_body_t* b, ForceType t){
  a->collisions[a->num_collisions_detected++] = b->collision_bounds;
}

void ReactionBumpForce(rigid_body_t* a, rigid_body_t* b, ForceType t){

  Rectangle collider = {
          .x =      a->collision_bounds.pos.x,
          .y =      a->collision_bounds.pos.y,
          .width =  a->collision_bounds.width,
          .height = a->collision_bounds.height,
          };

  Rectangle target = {
          .x =      b->collision_bounds.pos.x,
          .y =      b->collision_bounds.pos.y,
          .width =  b->collision_bounds.width,
          .height = b->collision_bounds.height,
          };

  Rectangle overlap;//TODO use this maybe instead of pen
  Vector2 surface_normal = GetNormalFromRecs(collider, target,&overlap);

  Vector2 angBetween = VectorDirectionBetween(b->position, a->position);

  float dist = Vector2Distance(b->position,a->position);

  float penAmount = CLAMPF((collider.width + target.width) - dist,0,collider.width);
  Vector2 penetration = Vector2Scale(angBetween,penAmount);
  
  Vector2 bump = Vector2Scale(Vector2Reflect(a->velocity,surface_normal), a->restitution);
  bump = Vector2Add(bump,Vector2Scale(b->velocity,b->restitution));

  float speed = Vector2Length(Vector2Add(a->velocity,b->velocity));
 if(Vector2Length(bump) == 0 && Vector2Length(penetration)==0)
  return;
 
 Vector2 rForce = Vector2Add(bump,penetration);

 if(Vector2DotProduct(rForce,Vector2Rotate(angBetween,PI))> 0)
   return;

 //TraceLog(LOG_INFO,"bump <%0.f,%0.f>",bump.x,bump.y); 
 //TraceLog(LOG_INFO,"pen <%0.f,%0.f>",penetration.x,penetration.y); 
 force_t reaction = ForceFromVector2(t,Vector2ClampValue(rForce,1,speed));

  // Apply impulse
  PhysicsApplyForce(a,reaction);

}

bool CheckCanSeeTarget(rigid_body_t* a, rigid_body_t *b, float range){
  if(PhysicsSimpleDistCheck(a,b) > range)
    return false;

  Ray2D ray = {
    a->position,
    VectorDirectionBetween(a->position,b->position),
    range
  };

  Rectangle bBounds = {
    .x = b->collision_bounds.pos.x,
    .y = b->collision_bounds.pos.y,
    .width = b->collision_bounds.width,
    .height = b->collision_bounds.height
  };

  if(RayIntersectsRec(ray,bBounds,&range))
    return true;

  return false;
}

