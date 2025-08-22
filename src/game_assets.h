#ifndef __GAME_ASSETS__
#define __GAME_ASSETS__

#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "ray_sheet.h"
#include "game_common.h"

#define MAX_SONGS 4

struct ent_s;
void InitResources();

typedef struct{
  Music music;
  float elapsed;
  float duration;
}music_track_t;

typedef struct{
  int           index;
  int           num_songs;
  music_track_t track[MAX_SONGS];
}music_group_t;

typedef enum{
  SFX_ALL,
  SFX_UI,
  SFX_ACTION,
  SFX_NONE
}SfxGroup;

typedef struct{
  int   num_sounds;
  Sound *sounds;
}sfx_group_t;

typedef struct{
  music_group_t   tracks;
  sfx_group_t     sfx[SFX_NONE];
  events_t        *timers[SFX_NONE];
}audio_manager_t;
void InitAudio();
void AudioStep();
void AudioPlayRandomSfx(SfxGroup group);

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
bool FreeSprite(sprite_t* s);
void DrawSprite(sprite_t* s);
//====SPRITE_T>>
#endif
