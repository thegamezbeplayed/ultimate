#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#define ROOM_WIDTH 1440
#define ROOM_HEIGHT 880

#define GRID_WIDTH 18
#define GRID_HEIGHT 11

#define CELL_WIDTH 80
#define CELL_HEIGHT 80

typedef struct {
    const char* engine_instance;
		const char* name;
		float x;
    float y;
		int sprite_sheet_index;
		int speed;
		int accel;
		int	team_enum;
} ObjectInstance;

static const ObjectInstance room_instances[] = {
    {"ent_data", "Mob Square", 1248, 128, 3, 16, 4, 1},
    {"ent_data", "Red Ball", 416, 288, 1, 16, 0, 0},
};

#define ROOM_INSTANCE_COUNT 2

#endif // ROOM_DATA_H
// Tile Data
typedef struct {
    int tile_index;
    int start_x;
		int start_y;
		int map_x;
    int map_y;
    int rotation;
    bool flip_x;
    bool flip_y;
} TileInstance;

static const TileInstance room_tiles[] = {
    {4, 0, 0, 0, 0, 0, 0, 0},
    {4, 0, 240, 0, 3, 0, 0, 0},
    {6, 0, 320, 0, 4, 0, 0, 0},
    {5, 0, 400, 0, 5, 0, 0, 0},
    {4, 0, 480, 0, 6, 0, 0, 0},
    {6, 0, 560, 0, 7, 0, 0, 0},
    {5, 0, 640, 0, 8, 0, 0, 0},
    {5, 80, 0, 1, 0, 0, 0, 0},
    {4, 160, 800, 2, 10, 0, 0, 0},
    {6, 240, 800, 3, 10, 0, 0, 0},
    {5, 320, 0, 4, 0, 0, 0, 0},
    {5, 320, 800, 4, 10, 0, 0, 0},
    {6, 400, 0, 5, 0, 0, 0, 0},
    {4, 400, 800, 5, 10, 0, 0, 0},
    {4, 480, 0, 6, 0, 0, 0, 0},
    {5, 560, 0, 7, 0, 0, 0, 0},
    {6, 640, 0, 8, 0, 0, 0, 0},
    {4, 640, 800, 8, 10, 0, 0, 0},
    {6, 720, 800, 9, 10, 0, 0, 0},
    {5, 800, 800, 10, 10, 0, 0, 0},
    {4, 880, 800, 11, 10, 0, 0, 0},
    {4, 960, 0, 12, 0, 0, 0, 0},
    {6, 960, 800, 12, 10, 0, 0, 0},
    {5, 1040, 0, 13, 0, 0, 0, 0},
    {5, 1040, 800, 13, 10, 0, 0, 0},
    {6, 1120, 0, 14, 0, 0, 0, 0},
    {4, 1120, 800, 14, 10, 0, 0, 0},
    {4, 1200, 0, 15, 0, 0, 0, 0},
    {6, 1200, 800, 15, 10, 0, 0, 0},
    {5, 1280, 0, 16, 0, 0, 0, 0},
    {6, 1360, 0, 17, 0, 0, 0, 0},
    {6, 1360, 80, 17, 1, 0, 0, 0},
    {5, 1360, 160, 17, 2, 0, 0, 0},
    {4, 1360, 480, 17, 6, 0, 0, 0},
    {6, 1360, 560, 17, 7, 0, 0, 0},
    {4, 1360, 720, 17, 9, 0, 0, 0},
    {4, 1360, 800, 17, 10, 0, 0, 0},
};

#define ROOM_TILE_COUNT 37
