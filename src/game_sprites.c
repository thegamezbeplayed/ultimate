#include "game_assets.h"
#include "game_utils.h"
#include "game_math.h"

sprite_sheet_data_t spritedata;
Texture2D sprite_sheet;

void InitResources(){
    LoadrtpAtlasSprite(&spritedata);
      Image spritesImg = LoadImage(TextFormat("resources/%s",ATLAS_RAY_SHEET_IMAGE_PATH));
  sprite_sheet = LoadTextureFromImage(spritesImg);

}

sprite_t* InitSprite(const char* group, sprite_sheet_data_t* spritesheet){
 sprite_t* spr =malloc(sizeof(sprite_t));
  memset(spr,0,sizeof(sprite_t));

  for(int i = 0; i < spritesheet->num_sprites; i++){
    if(strcmp(group,spritesheet->sprites[i]->group)!=0)
      continue;

    spr->sheet = &sprite_sheet; 
    spr->slice = spritesheet->sprites[i];

  }
  return spr;
}

sprite_t* InitSpriteByIndex(int index, sprite_sheet_data_t* spritesheet){
  sprite_t* spr =malloc(sizeof(sprite_t));
  memset(spr,0,sizeof(sprite_t));

  spr->mirror = false;

  spr->sheet = &sprite_sheet;
  spr->slice = spritesheet->sprites[index];

  return spr;
}

void DrawSlice(Texture2D tex, sprite_slice_t *slice, Vector2 position,bool mirror){
   Rectangle src = slice->bounds;
   if(mirror){
     src.width *=-1;
   }

    Rectangle dst = {
        position.x,
        position.y,
        slice->bounds.width * 1.0f,
        slice->bounds.height * 1.0f
    };

    Vector2 origin = (Vector2){
       (mirror ?slice->offset.x+slice->bounds.width - slice->center.x: slice->center.x-slice->offset.x),
       slice->center.y - slice->offset.y
    };
    DrawTexturePro(tex, src, dst, origin,0.0f, WHITE);
    return;
}

bool FreeSprite(sprite_t* s){
  if(!s) return false;

  /*if(s->slice)
    free(s->slice);
*/
  free(s);
  return true;
}

void DrawSprite(sprite_t* s){
  if(s->is_visible)
    DrawSlice(sprite_sheet,s->slice, s->pos,s->mirror);
}

void LoadrtpAtlasSprite(sprite_sheet_data_t *out){

  for (int i = 0; i < ATLAS_RAY_SHEET_SPRITE_COUNT; i++){
    rtpAtlasSprite sprData = rtpDescRaySheet[i];
 
    TraceLog(LOG_INFO,"Load index %d - %s into sprite sheet data",i,sprData.nameId);

   Vector2 center = Vector2FromXY(sprData.originX,sprData.originY);
    Vector2 offset = Vector2FromXY(sprData.trimRecX,sprData.trimRecY);
    Rectangle bounds = RecFromCoords(sprData.positionX,sprData.positionY,sprData.sourceWidth,sprData.sourceHeight);

    sprite_slice_t *spr = malloc(sizeof(sprite_slice_t));
    memset(spr,0,sizeof(sprite_slice_t));

    spr->name = (char*)malloc(MAX_NAME_LEN*sizeof(char));
    spr->group = (char*)malloc(MAX_NAME_LEN*sizeof(char));

    strcpy(spr->name,sprData.nameId);
    strcpy(spr->group,sprData.tag);
    spr->center = center;
    spr->offset = offset;
    spr->bounds = bounds;

    out->sprites[out->num_sprites++] = spr;

  }

  TraceLog(LOG_INFO,"Done with %s",ATLAS_RAY_SHEET_IMAGE_PATH);

}

