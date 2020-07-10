
#define MAP_WIDTH   (16)
#define MAP_HEIGHT  (16)

typedef struct Tile {
    i32         type;
} Tile;

typedef struct Map {
    Tile        tiles[MAP_HEIGHT][MAP_WIDTH];
} Map;

static void MapRender(Map* map);
static void MapUpdate(Map* map, f32 dt);
