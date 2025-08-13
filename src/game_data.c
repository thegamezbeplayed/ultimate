#include <raylib.h>
#include "game_utils.h"

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

JNode* ParseJNode(struct json_object* root){

  const char *name   = json_object_get_string(json_object_object_get(root, "name"));
  const char *type = json_object_get_string(json_object_object_get(root, "type"));

  JNode *node = malloc(sizeof(JNode));


  if(name){
    node->name = (char*)malloc(MAX_NAME_LEN*sizeof(char));
    node->name = strdup(name);
  }

  json_object *params_obj;
  if(json_object_object_get_ex(root, "params", &params_obj)&&
      params_obj && json_object_get_type(params_obj)== json_type_object){
  
    node->params = json_object_get(params_obj);
  }
  
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
