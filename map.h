
#define MAP_WIDTH   (32)
#define MAP_HEIGHT  (32)

#define BLOOD_SIZE          (16)
#define BLOOD_MAP_WIDTH     (BLOOD_SIZE * MAP_WIDTH)
#define BLOOD_MAP_HEIGHT    (BLOOD_SIZE * MAP_HEIGHT)

typedef struct ParticleSystem ParticleSystem;

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

    f32         heat;
    f32         fire;
} Tile;

static Texture  blood_texture;

typedef struct Map {
    Tile        tiles           [MAP_HEIGHT][MAP_WIDTH];
    int         path_to_player  [MAP_HEIGHT][MAP_WIDTH];
    
    v4          blood           [BLOOD_MAP_HEIGHT][BLOOD_MAP_WIDTH];
} Map;

static void MapInit     (Map* map);
static void MapRender   (Map* map);
static void MapUpdate   (Map* map, ParticleSystem* ps, f32 dt);

static void AddLight(Map* map, int source_x, int source_y, v3 light_color);
static void AddBlood(Map* map, f32 x, f32 y, f32 rad, v4 color);

