
// map stuff
static void MapInit(Map* map) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile* tile = &map->tiles[y][x];

            tile->type = iRand(0, 100) < 80? TILE_FLOOR : TILE_WALL;
        }
    }
}

static v3 GetWallLight(Map* map, int x, int y) {
    v3 result = {0};

    for (int i = 0; i < 4; ++i) {
        int nx = x + (int[]) { -1, 1, 0, 0 } [i];
        int ny = y + (int[]) { 0, 0, -1, 1 } [i];

        if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)    continue;
        if (map->tiles[ny][nx].type == TILE_WALL)                       continue;

        result = v3_Add(result, map->tiles[ny][nx].light);
    }

    return v3_Scale(result, 1.0f / 4.0f);
}

static void MapRender(Map* map) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile* tile = &map->tiles[y][x];

            switch (tile->type) {
                case TILE_FLOOR : {
                    v3 light = tile->light;

                    RenderRect((v2) { x + 0.5f, y + 0.5f }, 0.0f, (v2) { 0.5f, 0.5f }, (v4) { 0.3f * light.r, 0.3f * light.g, 0.3f * light.b, 1.0f });
                } break;
                case TILE_WALL: {
                    v3 light = GetWallLight(map, x, y);

                    RenderBox((v3) { x + 0.5f, y + 0.5f, 1.0f }, (v3) { 0.5f, 0.5f, 1.0f }, (v4) { 0.2f * light.r, 0.2f * light.g, 0.2f * light.b, 1.0f });
                } break;
            }
        }
    }

    {
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        TextureUpdateV4(blood_texture, map->blood, false);

        glBegin(GL_QUADS);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f,      0.0f,       0.1f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(MAP_WIDTH, 0.0f,       0.1f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(MAP_WIDTH, MAP_HEIGHT, 0.1f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,      MAP_HEIGHT, 0.1f);

        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
}

static void MapUpdate(Map* map, ParticleSystem* ps, f32 dt) {
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            Tile* tile = &map->tiles[y][x];

            tile->light = v3_Lerp(tile->light, (v3) {0}, 1.0f * dt);

            tile->heat = fClamp(tile->heat - dt, 0.0f, 8.0f);

            if (tile->heat <= 1.0f) {
                for (int i = 0; i < 4; ++i) {
                    int nx = x + (int[]) { -1, 1, 0, 0 } [i];
                    int ny = y + (int[]) { 0, 0, -1, 1 } [i];

                    if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT) continue;

                    Tile* nt = &map->tiles[ny][nx];

                    tile->heat += 0.5f * nt->heat * dt;
                }
            }

            if (tile->heat > 1.0f) {
                AddLight(map, x, y, (v3) { 1.0f, 1.0f, 0.0f });

                Particle particle = {
                    .pos        = { x + fRand(0.0f, 1.0f), y + fRand(0.0f, 1.0f) },
                    .vel        = v2_Rand(-2.0f, 2.0f),
                    .rad        = fRand(0.05f, 0.1f),
                    .life       = fRand(0.1f, 1.0f),
                    .max_life   = particle.life,
                    .col        = { 1.0f, 0.5f, 0.0f, 1.0f },
                };

                ParticleAdd(ps, &particle);
            }
        }
    }

    for (int y = 0; y < BLOOD_MAP_HEIGHT; ++y) {
        for (int x = 0; x < BLOOD_MAP_WIDTH; ++x) {
            v4* blood = &map->blood[y][x];

            blood->a = fLerp(blood->a, 0.05f, dt);

            int     count = 0;
            v4      color = {0};

            for (int i = 0; i < 4; ++i) {
                int nx = x + (int[]) { -1, 1, 0, 0 } [i];
                int ny = y + (int[]) { 0, 0, -1, 1 } [i];

                if (nx < 0 || nx >= BLOOD_MAP_WIDTH || ny < 0 || ny >= BLOOD_MAP_HEIGHT) continue;

                v4 n = map->blood[ny][nx];

                if (n.a > 0.1f) {
                    color = v4_Add(color, n);
                    count++;
                }
            }

            if (count > 2) {
                *blood = v4_Scale(color, 1.0f / count);
            }
        }
    }
}

#define LIGHT_DROP_OFF  (0.05f)

static void AddLight(Map* map, int source_x, int source_y, v3 light_color) {
    typedef struct Node {
        i32     x;
        i32     y;
        v3      light;
    } Node;

    static Node queue[MAP_WIDTH * MAP_HEIGHT];

    if (source_x < 0 || source_x >= MAP_WIDTH || source_y < 0 || source_y >= MAP_HEIGHT) { return; }

    light_color = v3_Clamp(light_color, 0.0f, 1.0f);

    u8 closed[MAP_HEIGHT][MAP_WIDTH] = {0};

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
            fClamp(current.light.r - LIGHT_DROP_OFF, 0.0f, 1.0f),
            fClamp(current.light.g - LIGHT_DROP_OFF, 0.0f, 1.0f),
            fClamp(current.light.b - LIGHT_DROP_OFF, 0.0f, 1.0f),
        };

        if (new_light.r + new_light.g + new_light.b <= 0.0f)
            return;

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + (int[]) { -1, 1, 0, 0 } [i];
            int ny = current.y + (int[]) { 0, 0, -1, 1 } [i];

            if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)    continue;
            if (map->tiles[ny][nx].type == TILE_WALL || closed[ny][nx])     continue;

            closed[ny][nx]  = true;
            queue[end++]    = (Node) { nx, ny, new_light };
        }
    }
}

static void AddBlood(Map* map, f32 px, f32 py, f32 rad, v4 color) {
    int blood_x     = px * BLOOD_SIZE;
    int blood_y     = py * BLOOD_SIZE;
    int blood_rad   = BLOOD_SIZE * rad;

    int sx = iClampMin(blood_x - blood_rad, 0);
    int sy = iClampMin(blood_y - blood_rad, 0);
    int ex = iClampMax(blood_x + blood_rad, BLOOD_MAP_WIDTH  - 1);
    int ey = iClampMax(blood_y + blood_rad, BLOOD_MAP_HEIGHT - 1);

    for (int y = sy; y <= ey; ++y) {
        for (int x = sx; x <= ex; ++x) {
            map->blood[y][x] = (v4) { color.r, color.g, color.b, 0.3f };
        }
    }
}

static void UpdatePathToPlayer(Map* map, int player_x, int player_y) {
    static int queue[MAP_WIDTH * MAP_HEIGHT];

    u8 closed[MAP_HEIGHT][MAP_WIDTH] = {0};

    for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        map->path_to_player[i / MAP_WIDTH][i % MAP_WIDTH] = i;
    }

    i32 begin   = 0;
    i32 end     = 0;

    map->path_to_player[player_y][player_x] = queue[end++] = player_y * MAP_WIDTH + player_x;
    closed[player_y][player_x] = true;

    while (begin < end) {
        int current = queue[begin++];

        int cx = current % MAP_WIDTH;
        int cy = current / MAP_WIDTH;

        for (int i = 0; i < 8; ++i) {
            int nx = cx + (int[]) { -1, +1, 0, 0, -1, +1, -1, +1 } [i];
            int ny = cy + (int[]) { 0, 0, -1, +1, -1, -1, +1, +1 } [i];

            if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)                        continue;
            if (map->tiles[ny][cx].type == TILE_WALL || map->tiles[cy][nx].type == TILE_WALL)   continue;
            if (map->tiles[ny][nx].type == TILE_WALL || closed[ny][nx])                         continue;

            closed[ny][nx] = true;

            map->path_to_player[ny][nx] = current;
            queue[end++] = ny * MAP_WIDTH + nx;
        }
    }
}

static v2 NextPathToPlayer(Map* map, int x, int y) {
    int next_tile = map->path_to_player[y][x];

    int nx = next_tile % MAP_WIDTH;
    int ny = next_tile / MAP_HEIGHT;

    return (v2) { nx + 0.5f, ny + 0.5f };
}

