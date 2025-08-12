#include <raylib.h>
#include "game_utils.h"
#include "game_process.h"

behavior_tree_node_t* BuildFromJNode(const JNode *jn) {
    if (jn->type == JNODE_LEAF) {
        // find leaf by name in registry; pass jn->params
        for (int i=0; i<g_bt_leaves_count; ++i) {
            if (strcmp(g_bt_leaves[i].name, jn->name) == 0) {
                // if your factories need params, change factory signature:
                // behavior_tree_node_t* (*factory)(behavior_params_t *params)
              behavior_params_t *params; 

              return g_bt_leaves[i].factory(params);
            }
        }
        return NULL; // unknown leaf
    }

    // composite:
    behavior_tree_node_t **kids = malloc(sizeof(*kids) * jn->child_count);
    for (int i=0; i<jn->child_count; ++i) {
        kids[i] = BuildFromJNode(jn->children[i]);
    }

    if (jn->type == JNODE_SEQUENCE)
      return BehaviorCreateSequence(kids, jn->child_count);
    else
      return BehaviorCreateSelector(kids, jn->child_count);
}

behavior_tree_node_t* InitBehaviorTree( const char* name){
  behavior_tree_node_t* node = BehaviorGetTree(name);

  if(node != NULL)
    return node;


  node = BuildFromJNode(raw_game_data);
  if(node == NULL){
    TraceLog(LOG_WARNING,"<=====Behavior Tree %s not found=====>",name);
    return NULL;
  }

  BehaviorAddTree(name,node);

  return node;
}

behavior_tree_node_t* BehaviorGetTree(const char *name){
  for (int i = 0; i < tree_cache_count; i++) {
    if (strcmp(tree_cache[i].name, name) == 0) {
      return tree_cache[i].root;
    }
  }
  return NULL;
}

void BehaviorAddTree(const char *name, behavior_tree_node_t *root){
  strncpy(tree_cache[tree_cache_count].name, name, sizeof(tree_cache[tree_cache_count].name)-1);
  tree_cache[tree_cache_count].root = root;
  tree_cache_count++;
}

BehaviorStatus BehaviorAcquireDestination(behavior_params_t *params){
   struct ent_s* e = params->owner;
  if(!e)
    return BEHAVIOR_FAILURE; 
}

BehaviorStatus BehaviorAcquireTarget(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e || !e->control)
    return BEHAVIOR_FAILURE;

  if(e->control->target)
    return BEHAVIOR_SUCCESS;

  struct ent_s* others[MAX_ENTS];
  int num_others =  WorldGetEnts(others,EntNotOnTeam, e);
  for (int i = 0; i < num_others; i++){
    if(CheckCanSeeTarget(e->body,others[i]->body, e->control->aggro)){
      e->control->target = others[i];
      TraceLog(LOG_INFO,"New target acquired");
      return BEHAVIOR_SUCCESS;
    }
  }
}

BehaviorStatus BehaviorCanSeeTarget(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e)
    return BEHAVIOR_FAILURE;
  return BEHAVIOR_FAILURE;
}

BehaviorStatus BehaviorMoveToTarget(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e || !e->control)
    return BEHAVIOR_FAILURE;

  if(PhysicsSimpleDistCheck(e->body,e->control->target->body) < e->control->range)
    return BEHAVIOR_SUCCESS;

  PhysicsAccelDir(e->body, FORCE_STEERING,Vector2Normalize(Vector2Subtract(e->control->target->pos,e->pos)));
  TraceLog(LOG_INFO,"Move %s towards %s at <%0.2f,%0.2f>",e->name,e->control->target->name,e->body->velocity.x,e->body->velocity.y);
  return BEHAVIOR_RUNNING;
}

BehaviorStatus BehaviorCanAttackTarget(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e)
    return BEHAVIOR_FAILURE;
  return BEHAVIOR_FAILURE;
}

BehaviorStatus BehaviorAttackTarget(behavior_params_t *params){
   struct ent_s* e = params->owner;
  if(!e)
    return BEHAVIOR_FAILURE;
  return BEHAVIOR_FAILURE;
}

BehaviorStatus BehaviorTickLeaf(behavior_tree_node_t *self, void *context) {
    behavior_tree_leaf_t *leaf = (behavior_tree_leaf_t *)self->data;
    if (!leaf || !leaf->action) return BEHAVIOR_FAILURE;
    return leaf->action(context);
}

behavior_tree_node_t* BehaviorCreateLeaf(BehaviorTreeLeafFunc fn, behavior_params_t* params){
  behavior_tree_leaf_t *data = malloc(sizeof(behavior_tree_leaf_t));

  data->action = fn;
  data->params = params;
  
  behavior_tree_node_t* node = malloc(sizeof(behavior_tree_node_t));
  node->bt_type = BT_LEAF;
  node->tick = BehaviorTickLeaf;
  node->data = data;

  return node;
}


behavior_tree_node_t* BehaviorCreateSequence(behavior_tree_node_t **children, int count) {
    behavior_tree_sequence_t *data = calloc(1,sizeof(behavior_tree_sequence_t));
    data->children = children;
    data->num_children = count;
    data->current = 0;

    behavior_tree_node_t *node = malloc(sizeof(behavior_tree_node_t));
    node->bt_type = BT_SEQUENCE;
    node->tick = BehaviorTickSequence;
    node->data = data;
    return node;
}

behavior_tree_node_t* BehaviorCreateSelector(behavior_tree_node_t **children, int count) {
    behavior_tree_selector_t *data = malloc(sizeof(behavior_tree_selector_t));
    data->children = children;
    data->num_children = count;
    data->current = 0;

    behavior_tree_node_t *node = malloc(sizeof(behavior_tree_node_t));
    node->bt_type = BT_SELECTOR;
    node->tick = BehaviorTickSelector;
    node->data = data;
    return node;
}

BehaviorStatus BehaviorTickSequence(behavior_tree_node_t *self, void *context) {
    behavior_tree_sequence_t *seq = (behavior_tree_sequence_t *)self->data;

    while (seq->current < seq->num_children) {
        BehaviorStatus status = seq->children[seq->current]->tick(seq->children[seq->current], context);
        if (status == BEHAVIOR_RUNNING) return BEHAVIOR_RUNNING;
        if (status == BEHAVIOR_FAILURE) {
            seq->current = 0;
            return BEHAVIOR_FAILURE;
        }
        seq->current++;
    }

    seq->current = 0;
    return BEHAVIOR_SUCCESS;
}

BehaviorStatus BehaviorTickSelector(behavior_tree_node_t *self, void *context) {
    behavior_tree_selector_t *sel = (behavior_tree_selector_t *)self->data;

    while (sel->current < sel->num_children) {
        BehaviorStatus status = sel->children[sel->current]->tick(sel->children[sel->current], context);
        if (status == BEHAVIOR_RUNNING) return BEHAVIOR_RUNNING;
        if (status == BEHAVIOR_SUCCESS) {
            sel->current = 0;
            return BEHAVIOR_SUCCESS;
        }
        sel->current++;
    }

    sel->current = 0;
    return BEHAVIOR_FAILURE;
}

