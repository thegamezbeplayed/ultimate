#include "game_types.h"

ent_t* InitEnt(ObjectInstance data){
  ent_t* e = malloc(sizeof(ent_t));
  *e = (ent_t){0};  // zero initialize if needed
  Vector2 pos = Vector2FromXY(data.x,data.y);
  e->pos = pos;
  e->name = (char*)malloc(100*sizeof(char));

  strcpy(e->name,data.name);
  e->sprite = InitSpriteByIndex(data.sprite_sheet_index,&spritedata);
    
  float radius = e->sprite->slice->bounds.height * 0.5f;
  e->body = InitRigidBody(e,pos, radius);
  SetState(e,STATE_SPAWN,NULL);
  e->team = data.team_enum;

  e->stats[STAT_SPEED] = InitStatOnMax(STAT_SPEED,data.speed);
  e->stats[STAT_ACCEL] = InitStatOnMax(STAT_ACCEL,data.accel);
  
  if(e->team == TEAM_ENEMIES){
    e->control = InitController(data);
    e->control->bt[STATE_IDLE] = InitBehaviorTree("Test");
  }
  SetState(e,STATE_SPAWN,OnStateChange);
  return e;
}

ent_t* InitEntStatic( TileInstance data){
  ent_t* e = malloc(sizeof(ent_t));
  *e = (ent_t){0};  // zero initialize if needed
  Vector2 pos = Vector2FromXY(data.start_x,data.start_y);
  e->pos = pos;

  e->sprite = InitSpriteByIndex(data.tile_index,&spritedata);
  if(e->sprite!=NULL){
    e->sprite->layer = LAYER_BG;
  }
  e->name = "tile";
  float radius = e->sprite->slice->bounds.height * 0.5f;
  e->body = InitRigidBodyStatic(e,pos, radius);
  e->team = TEAM_ENVIROMENT;
  return e;
}

stat_t InitStatEmpty(){
  return (stat_t){
    .attribute = STAT_BLANK,
      .min = 0,
      .max = 0,
      .current = 0,
      .increment = 0//TODO idk yet
  };
}

stat_t InitStatOnMax(StatType attr, float val){
  return (stat_t){
    .attribute = attr,
      .min = 0,
      .max = val,
      .current = val,
      .increment = 1//TODO idk yet
  };
}

void InitStats(stat_t stats[STAT_BLANK]){

}

void StatChangeValue(ent_t* owner, stat_t* attr, float val){
  float old = attr->current;
  attr->current+=val;
  attr->current = CLAMPF(attr->current,attr->min, attr->max);

  if(attr->current == old)
    return;

  if(attr->on_stat_change != NULL)
    attr->on_stat_change(owner);

  if(attr->current == attr->min && attr->on_stat_empty!=NULL)
    attr->on_stat_empty(owner);
}

controller_t* InitController(ObjectInstance data){
  controller_t* ctrl = malloc(sizeof(controller_t));
  *ctrl = (controller_t){0};

  ctrl->aggro = 200;
  ctrl->range = 80;
  return ctrl;
}

void EntSync(ent_t* e){
  if(!e->sprite)
    return;

  e->sprite->pos = e->pos;// + abs(ent->sprite->offset.y);

  switch(e->team){
    case TEAM_ENEMIES:
      EntControlStep(e);
      break;
    default:
      break;
  }
}

void EntControlStep(ent_t *e){
  if(!e->control || !e->control->bt || !e->control->bt[e->state])
    return;

  behavior_tree_node_t* current = e->control->bt[e->state];
  behavior_params_t param = {.owner = e};
  current->tick(current,&param);
}

bool SetState(ent_t *e, EntityState s,StateChangeCallback callback){
  if(CanChangeState(e->state,s)){
    EntityState old = e->state;
    e->state = s;

    if(callback!=NULL)
      callback(e,old,s);
    else
      OnStateChange(e,old,s);
    return true;
  }

  return false;
}

bool CanChangeState(EntityState old, EntityState s){
  if(old == s)
    return false;
  
  switch (COMBO_KEY(old,s)){
    case COMBO_KEY(STATE_NONE,!STATE_SPAWN):
    case COMBO_KEY(!STATE_NONE,STATE_SPAWN):
      return false;
      break;
    case COMBO_KEY(STATE_SPAWN,STATE_ATTACK):
      return false;
      break;
    default:
      return true;
      break;
  }
} 

void OnStateChange(ent_t *e, EntityState old, EntityState s){
  switch(old){
    case STATE_SPAWN:
      if(e->body)
        e->body->simulate=true;
      break;
    default:
      break;
  }

  switch(s){
    default:
      break;
  }

}

