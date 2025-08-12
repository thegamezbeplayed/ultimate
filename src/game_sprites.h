#ifndef __GAME_SPRITES__
#define __GAME_SPRITES__

#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "ray_sheet.h"

extern Texture2D sprite_sheet;

//forward declarations
struct ent_s;

typedef enum{
  LAYER_ROOT,
  LAYER_BG,
  LAYER_MAIN,
  LAYER_FLOOR,
  LAYER_FX_MAIN,
  LAYER_TOP
}RenderLayer;

typedef struct {
  char      *name;
  char      *group;
  int       sequence_index;
  Vector2   center;
  Rectangle bounds;
  Vector2   offset;
} sprite_slice_t;
void DrawSlice(Texture2D tex, sprite_slice_t *slice, Vector2 position,bool mirror);

typedef struct{
  int             num_sprites;
  sprite_slice_t  *sprites[1024];
}sprite_sheet_data_t;
extern sprite_sheet_data_t spritedata;
void LoadrtpAtlasSprite(sprite_sheet_data_t *out);
//SPRITE_T===>
typedef struct {
  int             suid;
  Texture2D       *sheet;
  sprite_slice_t* slice;
  //anim_t          *animation;
  //AnimType        active_anim;
  bool            mirror;
  bool            is_visible;
  Vector2         offset;
  Vector2         pos;
  RenderLayer     layer;
  struct ent_s    *owner;
} sprite_t;

sprite_t* InitSprite(const char* tag, sprite_sheet_data_t* spritesheet);
sprite_t* InitSpriteByIndex(int index, sprite_sheet_data_t* spritesheet);
void DrawSprite(sprite_t* s);
//====SPRITE_T>>
#endif
