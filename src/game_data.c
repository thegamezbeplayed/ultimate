#include <raylib.h>
#include "game_utils.h"

EntityState EntityStateLookup(const char* name){
  for (int i = 0; i < STATE_END; i++){
    if(strcmp(name, ent_state_alias[i].name) == 0)
      return ent_state_alias[i].state;
  }

  TraceLog(LOG_WARNING,"<====GAME_DATA====>\n====== State Definition for %s not implemented!",name);
  return 0;

}

const char* EntityStateName(EntityState s){
  for (int i = 0; i < STATE_END; i++){
    if(ent_state_alias[i].state == s)
      return ent_state_alias[i].name;
  }

  TraceLog(LOG_WARNING,"<====GAME_DATA====>\n====== State Definition for %d not implemented!",s);
  return 0;

}

//TODO might just load any json
void LoadJson(const char* filename,struct json_object** out){
  *out = json_object_from_file(filename);
  if (!*out){
    TraceLog(LOG_INFO,"<<<GAME_DATA>>>\n=== File / JSON not found: %s ===",filename);
    return;
  }

  TraceLog(LOG_INFO,"<<<GAME_DATA>>>\n=== JSON %s Loaded successfully ===",filename);

  //  fseek(fp, 0, SEEK_END)
}

static void load_one_tree(const char *name, json_object *jtree) {
    JNode *root = ParseJNode(jtree);        // your recursive parser
    if (root) BT_RegisterTree(name, root);
}

void LoadBehaviorTrees(json_object *root) {
    if (json_object_is_type(root, json_type_object)) {
        // Preferred: { "trees": { "Name": { ... }, ... } }
        json_object *trees = NULL;
        if (json_object_object_get_ex(root, "trees", &trees)
            && json_object_is_type(trees, json_type_object)) {
            json_object_object_foreach(trees, key, val) {
                load_one_tree(key, val);
            }
            return;
        }
        // Or: one big object where each top-level key is a tree
        json_object_object_foreach(root, key, val) {
            // skip non-tree keys like "version"
            if (strcmp(key, "version") == 0) continue;
            load_one_tree(key, val);
        }
    } else if (json_object_is_type(root, json_type_array)) {
        // Your current array form: [{ "name": "...", ... }, ...]
        int n = json_object_array_length(root);
        for (int i = 0; i < n; ++i) {
            json_object *jt = json_object_array_get_idx(root, i);
            json_object *jname = NULL;
            const char *name = NULL;
            if (json_object_object_get_ex(jt, "name", &jname))
                name = json_object_get_string(jname);
            if (!name || !*name) name = "UnnamedTree";
            load_one_tree(name, jt);
        }
    }
}

JNode* ParseJNode(struct json_object* root){
  const char *name   = json_object_get_string(json_object_object_get(root, "name"));
  const char *type = json_object_get_string(json_object_object_get(root, "type"));

  JNode *node = malloc(sizeof(JNode));


  if(name){
    node->name = (char*)malloc(MAX_NAME_LEN*sizeof(char));
    node->name = strdup(name);
  }

  json_object *params_obj = NULL;
  if(json_object_object_get_ex(root, "params", &params_obj)&&
      params_obj && json_object_get_type(params_obj)== json_type_object){
  
    node->params = json_object_get(params_obj);
  }
  else
    node->params = NULL;
  
  if (strcmp(type, "Leaf") == 0) {
    node->type = JNODE_LEAF;
    node->child_count = 0;  
  }
  else if (strcmp(type, "Sequence") == 0) {
    node->type = JNODE_SEQUENCE;
  }
  else if (strcmp(type, "Selector") == 0) {
    node->type = JNODE_SELECTOR;
  }
  
  if(node->type!=JNODE_LEAF){
    struct json_object *children = json_object_object_get(root, "children");
    int count = json_object_array_length(children);

    node->child_count = count;

    node->children = malloc(sizeof(*node->children) * count);

    for (int i = 0; i < count; i++) {
      struct json_object *child = json_object_array_get_idx(children, i);
      node->children[i] = ParseJNode(child);
    }

  }

  return node;
}
