#ifndef __GAME_TOOLS__
#define __GAME_TOOLS__
#include <float.h>
#include "game_math.h"

#define CLAMPV2(v,a,b) ((v)<(a)?(a):((v)>(b)?(b):(v)))
#define VEC_UNSET (Vector2){FLT_MAX, FLT_MAX}

static inline float frand() {
    return (float)rand() / (float)RAND_MAX;
}
static inline float v2_len(Vector2 v){ return sqrtf(v.x*v.x + v.y*v.y); }
static inline Vector2 v2_add(Vector2 a, Vector2 b){ return (Vector2){a.x+b.x,a.y+b.y}; }
static inline Vector2 v2_sub(Vector2 a, Vector2 b){ return (Vector2){a.x-b.x,a.y-b.y}; }
static inline Vector2 v2_scale(Vector2 a, float s){ return (Vector2){a.x*s,a.y*s}; }
static inline Vector2 v2_norm_safe(Vector2 v){
  float L = v2_len(v);
  return (L > 1e-6f) ? v2_scale(v, 1.0f/L) : (Vector2){1,0};
}


static inline int point_in_rect(Vector2 p, Rectangle r){
  return (p.x >= r.x && p.x <= r.x + r.width &&
      p.y >= r.y && p.y <= r.y + r.height);
}

static inline Vector2 clamp_point_to_rect(Vector2 p, Rectangle r){
  return (Vector2){
    CLAMPV2(p.x, r.x, r.x + r.width),
      CLAMPV2(p.y, r.y, r.y + r.height)
  };
}

// Random unit vector (use your RNG if needed)
static inline Vector2 rand_unit(){
  float a = ((float)rand() / (float)RAND_MAX) * 6.28318530718f;
  return (Vector2){cosf(a), sinf(a)};
}

// Distance you can move along dir before leaving rect (start assumed INSIDE).
// Returns a "t_max" so p + dir * t_max is the last point still inside (minus epsilon).
static float max_step_inside_rect(Vector2 p, Vector2 dir, Rectangle r){
  const float eps = 1e-4f;
  float tmax = FLT_MAX;

  if (fabsf(dir.x) > 1e-8f) {
    float tx = (dir.x > 0.0f)
      ? ((r.x + r.width) - p.x) / dir.x
      : ( (r.x) - p.x ) / dir.x;
    if (tx >= 0.0f) tmax = fminf(tmax, tx);
  }
  if (fabsf(dir.y) > 1e-8f) {
    float ty = (dir.y > 0.0f)
      ? ((r.y + r.height) - p.y) / dir.y
      : ( (r.y) - p.y ) / dir.y;
    if (ty >= 0.0f) tmax = fminf(tmax, ty);
  }
  if (tmax == FLT_MAX) tmax = 0.0f; // dir == (0,0)
  tmax = fmaxf(0.0f, tmax - eps);
  return tmax;
}
//TODO this doesnt do what i think..
static inline Vector2 GetNearbyDestination(Vector2 curr, float len, Rectangle area, float bias,float padding){
  // Clamp start to inside
  curr = clamp_point_to_rect(curr, area);

  // Random direction
  float angle = frand() * 6.28318530718f;
  Vector2 dir = (Vector2){cosf(angle), sinf(angle)};

  // Find max allowed in this dir
  float tmax = max_step_inside_rect(curr, dir, RectangleCrop(area,padding,padding));
  if (tmax <= 1e-5f) return curr; // No room to move

  // Choose step length: bias toward closer
  float dist_factor = frand(); // uniform 0..1
  dist_factor = powf(dist_factor, bias * 5.0f + 1.0f); 
  float step = fminf(len, tmax) * dist_factor;

  return v2_add(curr, v2_scale(dir, step));
}
#endif
