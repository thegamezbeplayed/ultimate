#ifndef __GAME_MATH__
#define __GAME_MATH__
#include "raymath.h"

#define CLAMP(x, low, high) (((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x)))
#define CLAMPF(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

typedef struct bounds_s bounds_t;
typedef struct {
    Vector2 origin;
    Vector2 direction; // should be normalized
    float length;
} Ray2D;
 
static inline Vector2 Vector2FromXY(float x, float y){
  Vector2 result = {
    x,y
  };

  return result;
}

static inline Vector2 Vector2FromAngle(float a, float len){
  return (Vector2){
    .x = cosf(a) * len,
    .y = sinf(a) * len
  };
}

static inline Rectangle RectangleCrop(Rectangle rec, float ex_wid,float ex_hei){
  return (Rectangle){
    .x = rec.x + ex_wid,
    .y = rec.y + ex_hei,
    .width = rec.width - ex_wid,
    .height = rec.height - ex_hei
  };
}

static inline Rectangle RecFromCoords(int x, int y, int width, int height){

  Rectangle result = {
    x,y,width,height
  };

  return result;
}

static inline Vector2 VectorDistanceBetween(Vector2 a, Vector2 b) {
  return Vector2Subtract(b,a);
}

static inline Vector2 VectorDirectionBetween(Vector2 a, Vector2 b) {
    return Vector2Normalize((Vector2){ b.x - a.x, b.y - a.y });
}
 
static inline Vector2 GetRecCenter(Rectangle rec){
  Vector2 result = {
    rec.x+rec.width/2.0f,
    rec.y+rec.height/2.0f
  };

  return result;
}
static inline Rectangle GetIntersectionRec(Rectangle r1, Rectangle r2){
  float x = fmaxf(r1.x, r2.x);
  float y = fmaxf(r1.y, r2.y);
  float w = fminf(r1.x + r1.width, r2.x + r2.width) - x;
  float h = fminf(r1.y + r1.height, r2.y + r2.height) - y;

  if (w <= 0 || h <= 0) return (Rectangle){0}; // no overlap
  return (Rectangle){ x, y, w, h };
}

static inline Vector2 GetNormalFromRecs(Rectangle collider, Rectangle target, Rectangle *out){
  const float threshold = 0.0f;

  Vector2 col_pos = GetRecCenter(collider);
  Vector2 tar_pos = GetRecCenter(target);

  Rectangle overlap = GetIntersectionRec(collider, target);

  Vector2 result = Vector2Zero();
  if (overlap.width < overlap.height - threshold) {
    if (col_pos.x > tar_pos.x){
//      TraceLog(LOG_INFO,"Hit from the left side of target\n");
      result.x = -1;
    }
    else{
  //    TraceLog(LOG_INFO,"Hit from the right side of target\n");
      result.x = 1;
    }
  }
  else if (overlap.height < overlap.width - threshold) {
    if (col_pos.y > tar_pos.y){
    //  TraceLog(LOG_INFO,"Hit from the top side of target\n");
      result.y = -1;
    }
    else{
      //TraceLog(LOG_INFO,"Hit from the bottom side of target\n");
      result.y = 1;
    }
  }

  *out = overlap;
  return result;
}
static inline bool RayIntersectsRec(Ray2D ray, Rectangle rect, float *dist) {
    float tmin = 0.0f;
    float tmax = ray.length;

    Vector2 invDir = {
        1.0f / (ray.direction.x != 0 ? ray.direction.x : 1e-6),
        1.0f / (ray.direction.y != 0 ? ray.direction.y : 1e-6)
    };

    Vector2 rectMin = { rect.x, rect.y };
    Vector2 rectMax = { rect.x + rect.width, rect.y + rect.height };

    for (int i = 0; i < 2; ++i) {
        float t1 = ((&rectMin.x)[i] - (&ray.origin.x)[i]) * (&invDir.x)[i];
        float t2 = ((&rectMax.x)[i] - (&ray.origin.x)[i]) * (&invDir.x)[i];

        if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }

        if (t2 < tmin || t1 > tmax)
            return false;

        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
    }

    if(tmin < 0.0f) return false;
    if (dist) *dist = tmin;
    return true;
}

#endif
