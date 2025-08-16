#include <raylib.h>
#include "game_utils.h"
#include "game_process.h"
#include "game_tools.h"

static bt_register_entry_t BT_Registry[MAX_BEHAVIOR_TREE];
static int registry_count = 0;

void BT_RegisterTree(const char *name, JNode *root) {
    if (registry_count < MAX_BEHAVIOR_TREE) {
        BT_Registry[registry_count].name = strdup(name);
        BT_Registry[registry_count].root = root;
        registry_count++;
    }
}

behavior_tree_node_t *BehaviorGetTree(const char *name) {
  for (int i = 0; i < registry_count; i++) {
    if (strcmp(BT_Registry[i].name, name) == 0){
      if(!BT_Registry[i].tree)
        BT_Registry[i].tree = BuildFromJNode(BT_Registry[i].root);

      return BT_Registry[i].tree;
    }
  }
  return NULL;
}

behavior_tree_node_t* BuildFromJNode(const JNode *jn) {
    if (jn->type == JNODE_LEAF) {
        // find leaf by name in registry; pass jn->params
        for (int i=0; i<g_bt_leaves_count; ++i) {
            if (strcmp(g_bt_leaves[i].name, jn->name) == 0) {
                // if your factories need params, change factory signature:
                // behavior_tree_node_t* (*factory)(behavior_params_t *params)
                behavior_params_t *params = BuildBehaviorParams(jn->params); 

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

behavior_params_t* BuildBehaviorParams(json_object* params){
  behavior_params_t* result = malloc(sizeof(behavior_params_t));
  *result = (behavior_params_t){0};

  if(params == NULL)
    return result; 

  json_object_object_foreach(params, key, val){
    const char* str = json_object_get_string(val);
    if(strcmp(key,"state") == 0)
      result->state = EntityStateLookup(str);

  }

  return result;
}

behavior_tree_node_t* InitBehaviorTree( const char* name){
  behavior_tree_node_t* node = BehaviorGetTree(name);

  if(node != NULL)
    return node;

  TraceLog(LOG_WARNING,"<=====Behavior Tree %s not found=====>",name);
  return NULL;
}

BehaviorStatus BehaviorChangeState(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e || !e->control)
    return BEHAVIOR_FAILURE;

  if(!params->state)
    return BEHAVIOR_FAILURE;

  SetState(e, params->state,NULL);
  TraceLog(LOG_INFO,"Change e %s state to %d",e->name, params->state);
  return BEHAVIOR_SUCCESS;

}

BehaviorStatus BehaviorAcquireDestination(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e || !e->control)
    return BEHAVIOR_FAILURE;

  if(!e->control->has_arrived && !v2_compare(e->control->destination,VEC_UNSET))
    return BEHAVIOR_SUCCESS;

  e->control->destination = GetWorldCoordsFromIntGrid(e->pos, e->control->aggro);
 
 TraceLog(LOG_INFO,"Ent %s move to <%0.2f,%0.2f>",e->name,e->control->destination.x,e->control->destination.y); 
  return BEHAVIOR_SUCCESS;
}

BehaviorStatus BehaviorMoveToDestination(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e || !e->control)
    return BEHAVIOR_FAILURE;

  if(e->control->has_arrived){
    e->control->has_arrived = false;
    return BEHAVIOR_SUCCESS;
  }
  if(Vector2Distance(e->pos,e->control->destination) <  e->control->range){
    e->control->has_arrived = true;
    return BEHAVIOR_SUCCESS;
  }

  PhysicsAccelDir(e->body, FORCE_STEERING,Vector2Normalize(Vector2Subtract(e->control->destination,e->pos)));
  return BEHAVIOR_RUNNING;

}

BehaviorStatus BehaviorAcquireTarget(behavior_params_t *params){
  struct ent_s* e = params->owner;
  if(!e || !e->control)
    return BEHAVIOR_FAILURE;

  if(EntTargetable(e->control->target))
    return BEHAVIOR_SUCCESS;

  e->control->target = NULL;

  struct ent_s* others[MAX_ENTS];
  int num_others =  WorldGetEnts(others,EntNotOnTeamAlive, e);
  for (int i = 0; i < num_others; i++){
    if(CheckCanSeeTarget(e->body,others[i]->body, e->control->aggro)){
      e->control->target = others[i];
      return BEHAVIOR_SUCCESS;
    }
  }

  return BEHAVIOR_FAILURE;
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

  if(!EntTargetable(e->control->target))
    return BEHAVIOR_FAILURE;

  if(PhysicsSimpleDistCheck(e->body,e->control->target->body) < e->control->range)
    return BEHAVIOR_SUCCESS;

  PhysicsAccelDir(e->body, FORCE_STEERING,Vector2Normalize(Vector2Subtract(e->control->target->pos,e->pos)));
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
    leaf->params->owner = context;
    return leaf->action(leaf->params);
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

