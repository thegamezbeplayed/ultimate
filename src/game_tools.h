#ifndef __GAME_TOOLS__
#define __GAME_TOOLS__
#include <float.h>
#include "game_math.h"

#define CLAMPV2(v,a,b) ((v)<(a)?(a):((v)>(b)?(b):(v)))
#define VEC_UNSET (Vector2){FLT_MAX, FLT_MAX}

typedef struct {
  int x,y;
} Cell;

static inline float distance(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrtf(dx*dx + dy*dy);
}

static inline float frand() {
    return (float)rand() / (float)RAND_MAX;
}
static inline Vector2 CellToVector2(Cell c, float scale){
  Vector2 result = Vector2FromXY(c.x,c.y);

  return Vector2Scale(result,scale);
}
static inline bool v2_compare(Vector2 v1,Vector2 v2){
  return (v1.x==v2.x && v1.y==v2.y);
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

  
  float angle = frand() * 6.28318530718f;

  Vector2 result = v2_add(curr,Vector2FromAngle(angle,len));

  return clamp_point_to_rect(result,RectangleCrop(area,padding,padding));
}

#endif
