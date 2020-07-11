
static void PushCollision(Entity* a, const Entity* b, f32 dt) {
    EntityApply(a, v2_Scale(v2_Norm(v2_Sub(a->pos, b->pos)), 16.0f));
}

static void DamageCollision(Entity* a, const Entity* b, f32 dt) {
    a->life -= 1.0f;
}

static void KillCollision(Entity* a, const Entity* b, f32 dt) {
    a->life = 0.0f;
}

typedef void CollisionFunc(Entity* a, const Entity* b, f32 dt);

static CollisionFunc* collision_table[ENTITY_COUNT][ENTITY_COUNT] = {
    // player collision:
    [ENTITY_PLAYER][ENTITY_ENEMY]   = PushCollision,
    [ENTITY_PLAYER][ENTITY_BULLET]  = DamageCollision,
    // enemy collision:
    [ENTITY_ENEMY][ENTITY_ENEMY]    = PushCollision,
    [ENTITY_ENEMY][ENTITY_PLAYER]   = PushCollision,
    [ENTITY_ENEMY][ENTITY_BULLET]   = DamageCollision,
    // bullet collision:
    [ENTITY_BULLET][ENTITY_PLAYER]  = KillCollision,
    [ENTITY_BULLET][ENTITY_ENEMY]   = KillCollision,
};

static void HandleCollision(GameState* gs, f32 dt) {
    EntityManager*  em  = &gs->entity_manager;
    Map*            map = &gs->map;
    
    for (int i = 0; i < em->count; ++i) {
        Entity* a = &em->array[i];

        if (map->tiles[(i32)(a->pos.y + a->rad)][(i32)(a->pos.x)].type == TILE_WALL) { a->vel.y = 0.0f; a->pos.y = ceilf(a->pos.y)  - a->rad; }
        if (map->tiles[(i32)(a->pos.y - a->rad)][(i32)(a->pos.x)].type == TILE_WALL) { a->vel.y = 0.0f; a->pos.y = floorf(a->pos.y) + a->rad; }
        if (map->tiles[(i32)(a->pos.y)][(i32)(a->pos.x - a->rad)].type == TILE_WALL) { a->vel.x = 0.0f; a->pos.x = floorf(a->pos.x) + a->rad; }
        if (map->tiles[(i32)(a->pos.y)][(i32)(a->pos.x + a->rad)].type == TILE_WALL) { a->vel.x = 0.0f; a->pos.x = ceilf(a->pos.x)  - a->rad; }

        for (int j = 0; j < em->count; ++j) {
            if (i == j) continue;

            const Entity* b = &em->array[j];

            if (a->id == b->owner_id) continue;
            if (a->owner_id == b->id) continue;

            if (EntityIntersect(a, b) && collision_table[a->type][b->type]) {
                collision_table[a->type][b->type](a, b, dt);
            }
        }
    }
}

static void CreateBullet(EntityManager* em, Entity* e, v2 aim) {
    Entity p = {0};
    p.type = ENTITY_BULLET;
    p.pos = v2_Add(e->pos, v2_Scale(aim, 0.1f));
    p.rad = 0.1f;
    p.life = 1.0f;
    p.powerup = e->powerup;
    p.owner_id  = e->id;
    f32 speed = 5.0f;
    if (p.powerup == POWERUP_SPEED) speed = 10.0f;
    else if (p.powerup == POWERUP_SLOW)  speed = 2.5f;
    p.vel = v2_Add(v2_Scale(aim, speed), e->vel);
    EntityAdd(em, &p);
}

static void UpdateEntities(GameState* gs, f32 dt) {
    EntityManager*  em  = &gs->entity_manager;
    Map*            map = &gs->map;
    
    static f32 powerup_switch_cooldown = 1.0f;
    powerup_switch_cooldown -= dt;
    for (int i = 0; i < em->count; ++i) {
        Entity* e = &em->array[i];
        
        b32 shoot = false;
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                v2 acc = {0};
                
                if (platform.key_down[GLFW_KEY_W]) acc.y += 1.0f;
                if (platform.key_down[GLFW_KEY_S]) acc.y -= 1.0f;
                if (platform.key_down[GLFW_KEY_A]) acc.x -= 1.0f;
                if (platform.key_down[GLFW_KEY_D]) acc.x += 1.0f;
                
                if (platform.mouse_down[GLFW_MOUSE_BUTTON_LEFT] && e->cooldown <= 0.0f) {
                    shoot = true;
                    e->aim = v2_Norm(v2_Sub(mouse_world_position.xy, e->pos));
                }
                
                EntityFriction(e, 4.0f);
                EntityApply(e, v2_Scale(v2_Norm(acc), 8.0f));
                
                AddLight(map, e->pos.x, e->pos.y, (v3) { 0.8f, 0.8f, 0.0f });
                
                gs->camera.target = (v3) {
                    .xy = v2_Add(e->pos, v2_Scale(e->vel, 0.8f)),
                    ._z = 12.0f + fClampMax(v2_Len(e->vel), 16.0f),
                };
                
                UpdatePathToPlayer(map, e->pos.x, e->pos.y);
            } break;
            case ENTITY_BULLET: {
                e->life -= dt;
            } break;
            case ENTITY_ENEMY: {
                v2 next_tile = NextPathToPlayer(map, e->pos.x, e->pos.y);
                
                v2 dir = v2_Norm(v2_Sub(next_tile, e->pos));
                
                EntityFriction(e, 4.0f);
                EntityApply(e, v2_Scale(dir, 6.0f));
                
                AddLight(map, e->pos.x, e->pos.y, (v3) { 0.8f, 0.0f, 0.1f });
                Entity* player = &em->array[0];
                v2 dist_vec = v2_Sub(player->pos, e->pos);
                if(e->cooldown <= 0.0f && v2_Len(dist_vec) < 2.0f) {
                    shoot = true;
                    e->aim = v2_Norm(dist_vec);
                }
            } break;
        }
        
        EntityUpdate(e, dt);
        
        if (shoot) {
            CreateBullet(em, e, e->aim);
            e->cooldown = powerup_cooldowns[e->powerup];
        }
        if (e->life <= 0.0f) EntityRemove(em, i);
        
        e->cooldown -= dt;
        if(e->type != ENTITY_BULLET && powerup_switch_cooldown <= 0.0f) {
            e->powerup = iRand(0, POWERUP_COUNT);
        }
    }
    if(powerup_switch_cooldown <= 0.0f) {
        powerup_switch_cooldown = fRand(2.0f, 10.0f);
    }
}

static void RenderEntities(EntityManager* em) {
    for (int i = 0; i < em->count; ++i) {
        Entity* e = &em->array[i];
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                RenderRect(e->pos, 0.1f, (v2) { e->rad, e->rad }, (v4) { 1.0f, 0.5f, 0, 1.0f });
            } break;
            case ENTITY_BULLET: {
                v4 color;
                switch(e->powerup) {
                    case POWERUP_NONE: {
                        color = (v4) { 0.0f, 0.0f, 0.0f, 1.0f };
                    } break;
                    case POWERUP_SPEED: {
                        color = (v4) { 0.0f, 1.0f, 0.0f, 1.0f };
                    } break;
                    case POWERUP_SLOW: {
                        color = (v4) { 0.0f, 0.0f, 1.0f, 1.0f };
                    } break;
                    case POWERUP_EXPLOSIVE: {
                        color = (v4) { 1.0f, 0.0f, 0.0f, 1.0f };
                    } break;
                    case POWERUP_FIRE: {
                        color = (v4) { 1.0f, 1.0f, 0.0f, 1.0f };
                    } break;
                    case POWERUP_CHARGE: {
                        color = (v4) { 1.0f, 1.0f, 1.0f, 1.0f };
                    } break;
                }
                
                RenderRect(e->pos, 0.1f, (v2) { e->rad, e->rad }, color);
            } break;
            case ENTITY_ENEMY: {
                RenderRect(e->pos, 0.1f, (v2) { e->rad, e->rad }, (v4) { 1.0f, 0.0f, 0.0f, 1.0f });
            } break;
        }
    }
}

static v2 GetValidSpawnLocation(Map* map) {
    int x = iRand(0, MAP_WIDTH);
    int y = iRand(0, MAP_HEIGHT);
    
    while (map->tiles[y][x].type != TILE_FLOOR) {
        x = iRand(0, MAP_WIDTH);
        y = iRand(0, MAP_HEIGHT);
    }
    
    return (v2) { x + 0.5f, y + 0.5f };
}

static void GameInit(GameState* gs) {
    *gs = (GameState) {0};
    
    MapInit(&gs->map);
    
    EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_PLAYER, .pos = GetValidSpawnLocation(&gs->map), .rad = 0.2f, .life = 1.0f, .powerup = POWERUP_NONE });
    
    for (int i = 0; i < 16; ++i) {
        EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_ENEMY, .pos = GetValidSpawnLocation(&gs->map), .rad = 0.2f, .life = 1.0f });
    }
}

static void GameRun(GameState* gs) {
    b32 game_running = true;
    
    GameInit(gs);
    
    Particle particle = {
        .pos    = { 0.0f, 0.0f },
        .vel    = { 0.1f, 0.1f },
        .rad    = 0.1f,
        .life   = 100.0f,
        .col    = { 1.0f, 1.0f, 1.0f, 1.0f },
    };
    
    ParticleAdd(&gs->particle_system, &particle);
    
    while (game_running) {
        f32 dt = platform.time_delta;
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE]) {
            game_running    = false;
        }
        
        MapUpdate(&gs->map, dt);
        
        AddLight(&gs->map, mouse_world_position.x, mouse_world_position.y, (v3) { 0.5f, 0.0f, 0.5f });
        
        HandleCollision(gs, dt);
        UpdateEntities(gs, dt);
        
        ParticlesUpdate(&gs->particle_system, dt);
        CameraUpdate(&gs->camera, dt);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // camera:
        {
            Camera* cam = &gs->camera;
            
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            
            gluPerspective(60.0f, platform.aspect, 0.1f, cam->current.z + 8.0f);
            gluLookAt(
                      cam->current.x, cam->current.y, cam->current.z,
                      cam->current.x, cam->current.y, 0.0f,
                      0.0f, 1.0f, 0.0f);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        
        MapRender(&gs->map);
        
        mouse_world_position = ToWorldPosition(
                                               iClamp(platform.mouse_position.x, 0, platform.width - 1),
                                               iClamp(platform.mouse_position.y, 1, platform.height));
        
        RenderEntities(&gs->entity_manager);
        ParticlesRender(&gs->particle_system);
        
        RenderBox(mouse_world_position, (v3) { 0.1f, 0.1f, 0.1f }, (v4) { 1.0f, 0.0f, 1.0f });
        
        PlatformUpdate();
    }
}

