#ifndef __GAME_UTIL__
#define __GAME_UTIL__

#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "game_common.h"

#define MAX_BEHAVIOR_TREE 8
#define MAX_NAME_LEN 64
#define COMBO_KEY(a, b) ((a << 8) | b)
#define CALL_FUNC(type, ptr, ...) ((type)(ptr))(__VA_ARGS__)
#define MAKE_ADAPTER(name, T) \
    static void name##_Adapter(void *p) { name((T)p); }
void LoadJson(const char* filename, struct json_object** out);
void LoadBehaviorTrees(json_object *root);

static inline void DO_NOTHING(void){}
//<===BEHAVIOR TREES

typedef enum { JNODE_LEAF, JNODE_SEQUENCE, JNODE_SELECTOR } JNodeType;

typedef struct JNode {
    JNodeType    type;
    const char   *name;     // for leaf
    json_object  *params;         // optional
    struct JNode **children;
    int          child_count;
} JNode;
JNode* ParseJNode(struct json_object* root);

//forward declare
typedef struct behavior_tree_node_s behavior_tree_node_t;

typedef struct {
    const char           *name;
    JNode                *root;
    behavior_tree_node_t *tree;
} bt_register_entry_t;

behavior_tree_node_t *BehaviorGetTree(const char *name);
void BT_RegisterTree(const char *name, JNode *root);

typedef enum{
  BEHAVIOR_SUCCESS,
  BEHAVIOR_FAILURE,
  BEHAVIOR_RUNNING
}BehaviorStatus;

typedef enum{
  BT_LEAF,
  BT_SEQUENCE,
  BT_SELECTOR
}BehaviorTreeType;

typedef struct {
    char name[32];
    behavior_tree_node_t *root;
} TreeCacheEntry;

static TreeCacheEntry tree_cache[16];
static int tree_cache_count = 0;

behavior_tree_node_t* BuildFromJNode(const JNode *jn);
static behavior_tree_node_t* BehaviorFindLeafFactory(const char *name);

typedef BehaviorStatus (*BehaviorTreeTickFunc)(behavior_tree_node_t* self, void* context);

typedef struct behavior_params_s{
  struct ent_s*  owner;
  EntityState    state;
}behavior_params_t;

behavior_params_t* BuildBehaviorParams(json_object* params);

typedef struct behavior_tree_node_s{
  BehaviorTreeType      bt_type;
  BehaviorTreeTickFunc  tick;
  void*                 data;
}behavior_tree_node_t;

typedef struct{
  behavior_tree_node_t  **children;
  int                   num_children;
  int                   current;
}behavior_tree_sequence_t;

typedef struct{
  behavior_tree_node_t  **children;
  int                   num_children;
  int                   current;
}behavior_tree_selector_t;

typedef BehaviorStatus (*BehaviorTreeLeafFunc)(behavior_params_t* params);

typedef struct{
  BehaviorTreeLeafFunc  action;
  behavior_params_t*    params;
}behavior_tree_leaf_t;

typedef struct {
    const char *name;  // "CanSeeTarget", "MoveToTarget", ...
    behavior_tree_node_t* (*factory)(behavior_params_t *params); // params is leaf-specific (can be NULL)
} BTLeafRegistryEntry;

behavior_tree_node_t* InitBehaviorTree( const char* name);
void FreeBehaviorTree(behavior_tree_node_t* node);
BehaviorStatus BehaviorTickSequence(behavior_tree_node_t *self, void *context);
BehaviorStatus BehaviorTickSelector(behavior_tree_node_t *self, void *context);
behavior_tree_node_t* BehaviorCreateLeaf(BehaviorTreeLeafFunc fn, behavior_params_t* params);
behavior_tree_node_t* BehaviorCreateSequence(behavior_tree_node_t **children, int count);
behavior_tree_node_t* BehaviorCreateSelector(behavior_tree_node_t **children, int count);
BehaviorStatus BehaviorTickSequence(behavior_tree_node_t *self, void *context);
BehaviorStatus BehaviorTickSelector(behavior_tree_node_t *self, void *context);

BehaviorStatus BehaviorChangeState(behavior_params_t *params);
BehaviorStatus BehaviorAcquireDestination(behavior_params_t *params);
BehaviorStatus BehaviorAcquireTarget(behavior_params_t *params);
BehaviorStatus BehaviorCanSeeTarget(behavior_params_t *params);
BehaviorStatus BehaviorMoveToTarget(behavior_params_t *params);
BehaviorStatus BehaviorMoveToDestination(behavior_params_t *params);
BehaviorStatus BehaviorCanAttackTarget(behavior_params_t *params);
BehaviorStatus BehaviorAttackTarget(behavior_params_t *params);

static inline behavior_tree_node_t* LeafChangeState(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorChangeState,params); }
static inline behavior_tree_node_t* LeafAcquireTarget(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorAcquireTarget,params); }
static inline behavior_tree_node_t* LeafAcquireDestination(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorAcquireDestination,params); }
static inline behavior_tree_node_t* LeafCanSeeTarget(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorCanSeeTarget,params); }
static inline behavior_tree_node_t* LeafMoveToTarget(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorMoveToTarget,params); }
static inline behavior_tree_node_t* LeafMoveToDestination(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorMoveToDestination,params); }
static inline behavior_tree_node_t* LeafCanAttackTarget(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorCanAttackTarget,params); }
static inline behavior_tree_node_t* LeafAttackTarget(behavior_params_t *params)  { return BehaviorCreateLeaf(BehaviorAttackTarget,params); }


static BTLeafRegistryEntry g_bt_leaves[] = {
    { "ChangeState",  LeafChangeState  },
    { "AcquireTarget",  LeafAcquireTarget  },
    { "AcquireDestination",  LeafAcquireDestination  },
    { "CanSeeTarget",  LeafCanSeeTarget  },
    { "MoveToTarget",  LeafMoveToTarget  },
    { "MoveToDestination",  LeafMoveToDestination  },
    { "CanAttackTarget",  LeafCanAttackTarget  },
    { "AttackTarget",  LeafAttackTarget  },
    // ...
};
static const int g_bt_leaves_count = sizeof(g_bt_leaves)/sizeof(g_bt_leaves[0]);
static behavior_tree_node_t* BehaviorFindLeafFactory(const char *name) {
    for (int i = 0; i < g_bt_leaves_count; ++i)
        if (strcmp(g_bt_leaves[i].name, name) == 0)
            return g_bt_leaves[i].factory(NULL); // call later w/ params
    return NULL;
}
#endif
