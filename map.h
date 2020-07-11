
#define MAP_WIDTH   (32)
#define MAP_HEIGHT  (32)

typedef u32 TileType;
enum {
    TILE_NONE,
    TILE_FLOOR,
    TILE_WALL,
    TILE_COUNT
};

typedef struct Tile {
    TileType    type;
    v3          light;
} Tile;

typedef struct Map {
    Tile        tiles[MAP_HEIGHT][MAP_WIDTH];
} Map;

static void MapInit     (Map* map);
static void MapRender   (Map* map);
static void MapUpdate   (Map* map, f32 dt);
