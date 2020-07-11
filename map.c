
// map stuff
static void MapInit(Map* map) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile* tile = &map->tiles[y][x];

            tile->type = iRand(0, 100) < 80? TILE_FLOOR : TILE_WALL;
        }
    }
}

static void MapRender(Map* map) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile* tile = &map->tiles[y][x];

            v3 light = tile->light;

            switch (tile->type) {
                case TILE_FLOOR : {
                    RenderRect((v2) { x + 0.5f, y + 0.5f }, 0.0f, (v2) { 0.5f, 0.5f }, (v4) { 0.3f * light.r, 0.3f * light.g, 0.3f * light.b, 1.0f });
                } break;
                case TILE_WALL: {
                    RenderBox((v3) { x + 0.5f, y + 0.5f, 0.5f }, (v3) { 0.5f, 0.5f, 0.5f }, (v4) { 0.2f * light.r, 0.2f * light.g, 0.2f * light.b, 1.0f });
                } break;
            }
        }
    }
}

static void MapUpdate(Map* map, f32 dt) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile* tile = &map->tiles[y][x];

            tile->light = v3_Lerp(tile->light, (v3) {0}, 8.0f * dt);
        }
    }
}

#define LIGHT_DROP_OFF  (0.1f)

static void AddLight(Map* map, int source_x, int source_y, v3 light_color) {
    typedef struct Node {
        i32     x;
        i32     y;
        v3      light;
    } Node;

    static Node queue[MAP_WIDTH * MAP_HEIGHT];
    static u8   closed[MAP_HEIGHT][MAP_WIDTH];

    memset(closed, 0, sizeof closed);

    int begin   = 0;
    int end     = 0;

    queue[end++] = (Node) { source_x, source_y, light_color };
    closed[source_y][source_x] = true;

    while (begin < end) {
        Node current = queue[begin++];

        // add light to map:
        {
            Tile* tile = &map->tiles[current.y][current.x];

            tile->light.r = fLerp(tile->light.r, current.light.r, current.light.r);
            tile->light.g = fLerp(tile->light.g, current.light.g, current.light.g);
            tile->light.b = fLerp(tile->light.b, current.light.b, current.light.b);
        }

        v3 new_light = {
            fClampMin(current.light.r - LIGHT_DROP_OFF, 0.0f),
            fClampMin(current.light.g - LIGHT_DROP_OFF, 0.0f),
            fClampMin(current.light.b - LIGHT_DROP_OFF, 0.0f),
        };

        if (new_light.r + new_light.g + new_light.b <= 0.0f)
            continue;

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + (int[]) { -1, 1, 0, 0 } [i];
            int ny = current.y + (int[]) { 0, 0, -1, 1 } [i];

            if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)    continue;
            if (map->tiles[ny][nx].type == TILE_WALL || closed[nx][ny])     continue;

            closed[ny][nx]  = true;
            queue[end++]    = (Node) { nx, ny, new_light };
        }
    }
}

