#include "game_process.h"

static interaction_t interactions[MAX_INTERACTIONS];
static bool interaction_used[MAX_INTERACTIONS]; 

const interaction_t default_interaction = {
  .source_uid = -1,
  .target_uid = -1,
  .timer = NULL
};

int InitInteractions(){
  int i = 0;
  for (i; i < MAX_INTERACTIONS; i++){
    interactions[i] = default_interaction;
    interaction_used[i] = false;
  }

  return i;
}

interaction_t* EntInteraction(unsigned int source, unsigned int target, int duration){
 interaction_t* result = malloc(sizeof(interaction_t));

 *result = (interaction_t){
   .source_uid = source,
   .target_uid = target,
   .timer = InitCooldown(duration,EVENT_INTERACTION,NULL,NULL)
 };

 return result;
}
cooldown_t* InitCooldown(int dur, EventType type, CooldownCallback on_end_callback, void* params){
  cooldown_t* cd = malloc(sizeof(cooldown_t)); 

  *cd = (cooldown_t){
    .type = type,
      .is_complete = false,
      .duration = dur,
      .elapsed = 0,
      .on_end_params = params,
      .on_end = on_end_callback// ? on_end_callback : DO_NOTHING
  };
  
  return cd;
}



bool AddInteraction(interaction_t* inter){
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(interaction_used[i])
      continue;

    interaction_used[i] = true;

    interactions[i] = *inter;

    return true;
  }

  return false;
}

bool CanInteract(int source, int target){
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(interactions[i].source_uid != source)
      continue;

    if(interactions[i].target_uid == target)
      return false;
  }

  return true;
}

void FreeInteraction(interaction_t* item) {
  int index = item - interactions;
  if (index >= 0 && index < MAX_INTERACTIONS) {
    FreeInteractionByIndex(index);
  }
}

void FreeInteractionByIndex(int i) {
  if (i < 0 && i > MAX_INTERACTIONS)
    return;

  interaction_used[i] = false;
  interactions[i] = default_interaction;
}

void InteractionStep(){
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(!interaction_used[i])
      continue;

    if(interactions[i].timer == NULL){
      FreeInteractionByIndex(i);
      continue;
    }

    if(interactions[i].timer->elapsed >= interactions[i].timer->duration){
      FreeInteractionByIndex(i);
      continue;
    }

    interactions[i].timer->elapsed++;
  }
}

bool AddEvent(events_t* pool, cooldown_t* cd){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(pool->cooldown_used[i]){
      if(pool->cooldowns[i].type == EVENT_NONE){
        pool->cooldowns[i] = *cd;
        pool->cooldown_used[i] = true;
        return true;
      }
      else
        continue;
    }
    else{
      pool->cooldowns[i] = *cd;
      pool->cooldown_used[i] = true;
      return true;
    }
  }

  return false;
}

events_t* InitEvents(){
  events_t* ev = malloc(sizeof(events_t));
  *ev =  (events_t) { 0 };

  for(int i = 0; i < MAX_EVENTS; i++){
    ev->cooldown_used[i] = false;
    ev->cooldowns[i].type = EVENT_NONE;
  }

  return ev;
}

void StepEvents(events_t* pool){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(!pool->cooldown_used[i])
      continue;

    if(pool->cooldowns[i].type == EVENT_NONE)
      continue;

    if(pool->cooldowns[i].is_complete){
      pool->cooldowns[i]=(cooldown_t){0};
      pool->cooldowns[i].type = EVENT_NONE;
      pool->cooldown_used[i] = false;
      continue;
    }
    if(pool->cooldowns[i].elapsed >= pool->cooldowns[i].duration){
      TraceLog(LOG_INFO,"Cooldown %i ended",pool->cooldowns[i].type);
      pool->cooldowns[i].is_complete = true;
      pool->cooldowns[i].elapsed = 0;
      pool->cooldowns[i].on_end(pool->cooldowns[i].on_end_params);
      continue;
    }

    pool->cooldowns[i].elapsed++;

  }
}

