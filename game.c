
static i32 zombies_killed;
static b32 health_pack_active;
static b32 help_screen_bool;

#define PLAYER_HEALTH 10.0f

static void PushCollision(Entity* a, const Entity* b, f32 dt) {
    EntityApply(a, v2_Scale(v2_Norm(v2_Sub(a->pos, b->pos)), 16.0f));
}

static void DamageCollision(Entity* a, const Entity* b, f32 dt) {
    EntityPush(a, v2_Scale(v2_Norm(b->vel), 2.0));
    
    a->life -= 1.0f;
}

static void KillCollision(Entity* a, const Entity* b, f32 dt) {
    a->life = 0.0f;
}

typedef void CollisionFunc(Entity* a, const Entity* b, f32 dt);

static CollisionFunc* collision_table[ENTITY_COUNT][ENTITY_COUNT] = {
    // player collision:
    [ENTITY_PLAYER][ENTITY_ENEMY]   = DamageCollision,
    [ENTITY_PLAYER][ENTITY_BULLET]  = DamageCollision,
    [ENTITY_PLAYER][ENTITY_CORPSE]  = PushCollision,
    // enemy collision:
    [ENTITY_ENEMY][ENTITY_ENEMY]    = PushCollision,
    [ENTITY_ENEMY][ENTITY_PLAYER]   = PushCollision,
    [ENTITY_ENEMY][ENTITY_BULLET]   = DamageCollision,
    [ENTITY_ENEMY][ENTITY_CORPSE]   = PushCollision,
    // bullet collision:
    [ENTITY_BULLET][ENTITY_PLAYER]  = KillCollision,
    [ENTITY_BULLET][ENTITY_ENEMY]   = KillCollision,
    [ENTITY_BULLET][ENTITY_BULLET]  = KillCollision,
    // corpse collision
    [ENTITY_CORPSE][ENTITY_PLAYER]  = PushCollision,
    [ENTITY_CORPSE][ENTITY_ENEMY]   = PushCollision,
    [ENTITY_CORPSE][ENTITY_CORPSE]  = PushCollision,
    //area collision:
    [ENTITY_ENEMY][ENTITY_AREA_DMG] = KillCollision,
    //health pack collision
    [ENTITY_HEALTH_PACK][ENTITY_PLAYER] = KillCollision,
};

static void HandleCollision(GameState* gs, f32 dt) {
    EntityManager*  em  = &gs->entity_manager;
    Map*            map = &gs->map;
    
    for (int i = 0; i < em->count; ++i) {
        Entity* a = &em->array[i];
        
        a->flags = a->flags & ~(ENTITY_FLAG_IMPACT | ENTITY_FLAG_DAMAGE);
        
        b32 impact = false;
        
        if ((a->pos.x - a->rad) < 0)            { impact = true; a->pos.x = a->rad; }
        if ((a->pos.y - a->rad) < 0)            { impact = true; a->pos.y = a->rad; }
        if ((a->pos.x + a->rad) >= MAP_WIDTH)   { impact = true; a->pos.x = MAP_WIDTH  - a->rad; }
        if ((a->pos.y + a->rad) >= MAP_HEIGHT)  { impact = true; a->pos.y = MAP_HEIGHT - a->rad; }
        
        if (map->tiles[(i32)(a->pos.y + a->rad)][(i32)(a->pos.x)].type == TILE_WALL) { impact = true; a->vel.y = 0.0f; a->pos.y = ceilf(a->pos.y)  - a->rad; }
        if (map->tiles[(i32)(a->pos.y - a->rad)][(i32)(a->pos.x)].type == TILE_WALL) { impact = true; a->vel.y = 0.0f; a->pos.y = floorf(a->pos.y) + a->rad; }
        if (map->tiles[(i32)(a->pos.y)][(i32)(a->pos.x - a->rad)].type == TILE_WALL) { impact = true; a->vel.x = 0.0f; a->pos.x = floorf(a->pos.x) + a->rad; }
        if (map->tiles[(i32)(a->pos.y)][(i32)(a->pos.x + a->rad)].type == TILE_WALL) { impact = true; a->vel.x = 0.0f; a->pos.x = ceilf(a->pos.x)  - a->rad; }
        
        f32 life = a->life;
        
        for (int j = 0; j < em->count; ++j) {
            if (i == j) continue;
            
            const Entity* b = &em->array[j];
            
            if (a->id == b->owner_id)       continue;
            if (a->owner_id == b->id)       continue;
            if (a->owner_id && a->owner_id == b->owner_id) continue;
            
            if (EntityIntersect(a, b) && collision_table[a->type][b->type]) {
                impact = true;
                collision_table[a->type][b->type](a, b, dt);
            }
        }
        
        if (life != a->life) {
            a->flags |= ENTITY_FLAG_DAMAGE;
        }
        
        if (impact) {
            a->flags |= ENTITY_FLAG_IMPACT;
        }
    }
}

static void CreateBullet(EntityManager* em, Entity* e, v2 aim) {
    if(e->powerup == POWERUP_MELEE) {
        EntityAdd(em, &(Entity) { .type = ENTITY_AREA_DMG, .pos = v2_Add(e->pos, v2_Scale(e->aim, 0.6f)), .rad = 0.3f, .life = 0.1f });
    } else {
        f32 speed = 5.0f;
        i32 shots = 1;
        
        if      (e->powerup == POWERUP_SPEED)   speed = 10.0f;
        else if (e->powerup == POWERUP_SHOTGUN) shots = 5;
        
        for(int i = 0; i < shots; i++) {
            Entity p = {
                .type       = ENTITY_BULLET,
                .pos        = v2_Add(e->pos, v2_Scale(aim, 0.1f)),
                .rad        = 0.1f,
                .life       = 1.0f,
                .powerup    = e->powerup,
                .owner_id   = e->id,
                .vel        = v2_Add(v2_Add(v2_Scale(aim, speed), e->vel), v2_Rand(-1.0f, 1.0f)),
            };
            
            EntityAdd(em, &p);
        }
    }
    
    switch (e->powerup) {
        case POWERUP_NONE:      AudioPlay(AUDIO_GUN_0);        break;
        case POWERUP_SPEED:     AudioPlay(AUDIO_GUN_1);        break;
        case POWERUP_SHOTGUN:   AudioPlay(AUDIO_SHOTGUN);      break;
        case POWERUP_FIRE:      AudioPlay(AUDIO_FLAMETHROWER); break;
        case POWERUP_MELEE:     AudioPlay(AUDIO_KNIFE);        break;
        case POWERUP_EXPLOSIVE: AudioPlay(AUDIO_MISSILE);      break;
    }
}

static v2 GetValidSpawnLocation(Map* map) {
    int x = iRand(0, MAP_WIDTH);
    int y = iRand(0, MAP_HEIGHT);
    
    while (map->tiles[y][x].type != TILE_FLOOR || map->path_to_player[y][x] == y * MAP_WIDTH + x) {
        x = iRand(0, MAP_WIDTH);
        y = iRand(0, MAP_HEIGHT);
    }
    
    return (v2) { x + 0.5f, y + 0.5f };
}

static f32 powerup_switch_cooldown = 0.0f;

static f32 player_health;
static i32 player_powerup;

static void UpdateEntities(GameState* gs, f32 dt) {
    EntityManager*  em  = &gs->entity_manager;
    ParticleSystem* ps  = &gs->particle_system;
    Map*            map = &gs->map;
    Camera*         cam = &gs->camera;
    
    if (powerup_switch_cooldown <= 0.0f) {
        powerup_switch_cooldown = fRand(2.0f, 10.0f);
    }
    
    powerup_switch_cooldown -= dt;
    
    for (int i = 0; i < em->count; ++i) {
        Entity* e = &em->array[i];
        
        Tile* tile = &map->tiles[(int)e->pos.y][(int)e->pos.x];
        
        // handle fire
        {
            e->fire = fClampMin(e->fire - dt, 0.0f);
            
            if (tile->heat >= 1.0f) {
                e->fire = 1.0f;
            }
            
            if (e->fire > 0.0f) {
                e->life     -= dt;
                tile->heat  += 0.3f * dt;
                
                Particle particle = {
                    .pos        = { e->pos.x + fRand(-e->rad, e->rad), e->pos.y + fRand(-e->rad, e->rad) },
                    .vel        = { .xy = v2_Rand(-2.0f, 2.0f) },
                    .rad        = fRand(0.05f, 0.1f),
                    .life       = fRand(0.2f, 0.5f),
                    .max_life   = particle.life,
                    .col        = { 1.0f, 0.5f, 0.0f, 1.0f },
                };
                
                ParticleAdd(ps, &particle);
            }
        }
        
        b32 shoot = false;
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                v2 acc = {0};
                e->cooldown -= dt;
                if(powerup_switch_cooldown <= 0.0f) {
                    e->powerup = iRand(0, POWERUP_COUNT);
                }
                
                if(e->life < PLAYER_HEALTH * 0.75f && !health_pack_active) {
                    health_pack_active = true;
                    v2 pos = GetValidSpawnLocation(&gs->map);
                    EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_HEALTH_PACK, .pos = pos, .rad = 0.1f, .life = 1.0f });
                }
                
                if (platform.key_down[GLFW_KEY_W]) acc.y += 1.0f;
                if (platform.key_down[GLFW_KEY_S]) acc.y -= 1.0f;
                if (platform.key_down[GLFW_KEY_A]) acc.x -= 1.0f;
                if (platform.key_down[GLFW_KEY_D]) acc.x += 1.0f;
                
                if (platform.key_pressed[GLFW_KEY_SPACE]) {
                    EntityPush(e, v2_Scale(v2_Norm(acc), 4.0f));
                }
                
                v2 mouse_vec = v2_Sub(mouse_world_position.xy, e->pos);
                
                if (e->powerup != POWERUP_MELEE) {
                    e->aim = v2_Scale(v2_Norm(mouse_vec),
                                      fClamp(fLerp(0.5, 1.0f, 1.0f - e->cooldown / powerup_cooldowns[e->powerup]), 0.5f, 1.0f));
                } else {
                    e->aim = v2_Scale(v2_Norm(mouse_vec),
                                      fClamp(fLerp(1.0, 1.5f, e->cooldown / powerup_cooldowns[e->powerup]), 1.0f, 1.5f));
                }
                
                if (platform.mouse_down[GLFW_MOUSE_BUTTON_LEFT] && e->cooldown <= 0.0f) {
                    shoot       = true;
                    cam->shake  += 0.1f;
                }
                
                if (shoot) {
                    CreateBullet(em, e, e->aim);
                    
                    e->cooldown = powerup_cooldowns[e->powerup];
                }
                
                if (e->flags & ENTITY_FLAG_DAMAGE) {
                    BloodSpurter(ps, e, 16);
                }
                
                EntityFriction(e, 4.0f);
                EntityApply(e, v2_Scale(v2_Norm(acc), 8.0f));
                
                AddLight(map, e->pos.x, e->pos.y, (v3) { 0.8f, 0.8f, 0.0f });
                
                cam->target = (v3) {
                    .xy = v2_Add(v2_Add(e->pos, v2_Scale(e->vel, 0.8f)), v2_Scale(mouse_vec, 0.3f)),
                    ._z = 8.0f + fClampMax(v2_Len(e->vel), 2.0f),
                };
                
                UpdatePathToPlayer(map, e->pos.x, e->pos.y);
                
                player_health   = e->life;
                player_powerup  = e->powerup;
            } break;
            case ENTITY_BULLET: {
                if (e->flags & ENTITY_FLAG_IMPACT && e->powerup != POWERUP_FIRE) {
                    for (int i = 0; i < 32; ++i) {
                        Particle p = CreateParticle(e->pos, v2_Rand(-2.0f, 2.0f), 0.02f, 0.5f, powerup_colors[e->powerup]);
                        
                        ParticleAdd(&gs->particle_system, &p);
                    }
                    
                    e->life = 0.0f;
                }
                
                e->life -= dt;
                
                if(e->powerup == POWERUP_FIRE) {
                    AddLight(map, e->pos.x, e->pos.y, (v3) { 1.0f, 1.0f, 0.0f });
                    Particle particle = {
                        .pos        = { e->pos.x + fRand(-0.2f, 0.2f), e->pos.y + fRand(-0.2f, 0.2f) },
                        .vel        = { .xy = v2_Rand(-2.0f, 2.0f) },
                        .rad        = fRand(0.05f, 0.1f),
                        .life       = fRand(0.1f, 1.0f),
                        .max_life   = particle.life,
                        .col        = { 1.0f, 0.5f, 0.0f, 1.0f },
                    };
                    
                    ParticleAdd(ps, &particle);
                }
                else {
                    for(int loop = 0; loop < 10; loop++) {
                        Particle p = CreateParticle(e->pos, v2_Sub(v2_Rand(-5.0f, 5.0f), e->vel), 0.01f, 0.1f, powerup_colors[e->powerup]);
                        ParticleAdd(&gs->particle_system, &p);
                    }
                }
            } break;
            case ENTITY_ENEMY: {
                v2 next_tile = NextPathToPlayer(map, e->pos.x, e->pos.y);
                
                v2 dir = v2_Norm(v2_Sub(next_tile, e->pos));
                
                if (e->flags & ENTITY_FLAG_DAMAGE) {
                    BloodSpurter(ps, e, 16);
                }
                
                EntityFriction(e, 4.0f);
                EntityApply(e, v2_Scale(dir, 6.0f));
                
                AddLight(map, e->pos.x, e->pos.y, (v3) { 0.8f, 0.0f, 0.1f });
                Entity* player = &em->array[0];
                v2 dist_vec = v2_Sub(player->pos, e->pos);
                
                if (e->cooldown <= 0.0f && v2_Len(dist_vec) < 2.0f) {
                    shoot = true;
                    e->aim = v2_Norm(dist_vec);
                }
            } break;
            case ENTITY_CORPSE: {
                BloodSpurter(&gs->particle_system, e, 1);
                EntityFriction(e, 8.0f);
                e->life -= dt;
            } break;
            case ENTITY_AREA_DMG: {
                e->life -= dt;
            } break;
        }
        
        EntityUpdate(e, dt);
        
        if (e->life <= 0.0f) {
            if (e->type == ENTITY_PLAYER) {
                AudioPlay(AUDIO_SCREAM);
            } else if (e->type == ENTITY_BULLET) {
                if (e->powerup == POWERUP_EXPLOSIVE) {
                    ParticleExplosion(&gs->particle_system, e->pos, 0.05f, 1000, 5.0f);
                    EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_AREA_DMG, .pos = e->pos, .rad = 1.0f, .life = 0.1f });
                    
                    tile->heat = 2.0f;
                    
                    AudioPlayFromSource(AUDIO_EXPLOSION, cam->current.xy, e->pos, 1.0f);
                }
                
                if (e->powerup == POWERUP_FIRE) {
                    map->tiles[(i32)e->pos.y][(i32)e->pos.x].heat = 1.5f;
                }
                
                if (e->flags & ENTITY_FLAG_IMPACT && e->powerup != POWERUP_FIRE) {
                    AudioPlayFromSource(AUDIO_IMPACT, cam->current.xy, e->pos, 1.0f);
                }
            } else if (e->type == ENTITY_CORPSE) {
                BloodExplosion(&gs->particle_system, e, 100);
            } else if(e->type == ENTITY_PLAYER) {
                EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_CORPSE, .pos = e->pos, .aim = v2_Norm(e->vel), .rad = e->rad, .life = 2.0f });
            } else if(e->type == ENTITY_ENEMY) {
                EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_CORPSE, .pos = e->pos, .aim = v2_Norm(e->vel), .rad = e->rad, .life = 2.0f });
                zombies_killed++;
            } else if(e->type == ENTITY_HEALTH_PACK) {
                Entity* player = &em->array[0];
                player->life = PLAYER_HEALTH;
                health_pack_active = false;
            }
            
            EntityRemove(em, i);
        }
    }
}

static void RenderEntities(const EntityManager* em, const Map* map) {
    for (int i = 0; i < em->count; ++i) {
        const Entity* e    = &em->array[i];
        const Tile*   tile = &map->tiles[(int)e->pos.y][(int)e->pos.x];
        
        v3 light = tile->light;
        
        f32 speed = v2_Len(e->vel);
        
        if (e->flags & ENTITY_FLAG_DAMAGE) {
            light = (v3) { 1.0f, 1.0f, 1.0f };
        }
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                v3 player_pos = { e->pos.x, e->pos.y, 0.3f } ;
                v3 weapon_pos = v3_Add(player_pos, (v3) { .xy = v2_Scale(e->aim, 0.4f), ._z = 0.1f });
                
                RenderTexture(player_texture, player_pos, e->rad, 0.1f * sinf(speed * platform.time_total),  (v4) { light.r, light.g, light.b, 1.0f });
                
                switch (e->powerup) {
                    default: {
                        RenderTexture(gun_texture, weapon_pos, e->rad, v2_GetAngle((v2) { -1.0f, 0.0f }, e->aim), (v4) { light.r, light.g, light.b, 1.0f });
                    } break;
                    case POWERUP_SPEED: {
                        RenderTexture(usi_texture, weapon_pos, e->rad, v2_GetAngle((v2) { -1.0f, 0.0f }, e->aim), (v4) { light.r, light.g, light.b, 1.0f });
                    } break;
                    case POWERUP_FIRE: {
                        RenderTexture(flame_texture, weapon_pos, e->rad, v2_GetAngle((v2) { -1.0f, 0.0f }, e->aim), (v4) { light.r, light.g, light.b, 1.0f });
                    } break;
                    case POWERUP_EXPLOSIVE: {
                        RenderTexture(launcher_texture, weapon_pos, e->rad, v2_GetAngle((v2) { -1.0f, 0.0f }, e->aim), (v4) { light.r, light.g, light.b, 1.0f });
                    } break;
                    case POWERUP_MELEE: {
                        RenderTexture(knife_texture, weapon_pos, e->rad, v2_GetAngle((v2) { -1.0f, 0.0f }, e->aim), (v4) { light.r, light.g, light.b, 1.0f });
                    } break;
                }
                
#if 0
                RenderRect(
                           v2_Add(e->pos, (v2) {0.0f, 0.5f}), 1.0f,
                           (v2) {(powerup_switch_cooldown / powerup_switch_cooldown_org) * 0.3f, 0.05f }, (v4) { 0.0f, 0.0f, 1.0f, 1.0f });
                
                RenderRect(v2_Add(e->pos, (v2) { 0.0f, -0.5f }), 2.0f,
                           (v2) {(e->life / PLAYER_HEALTH) * 0.3f, 0.05f }, (v4) { 0.0f, 1.0f, .0f, 1.0f });
                
                RenderRect(v2_Add(e->pos, (v2) {0.0f, -0.5f}), 2.0f,
                           (v2) { 0.3f, 0.05f }, (v4) { 1.0f, 0.0f, .0f, 1.0f });
#endif
            } break;
            case ENTITY_BULLET: {
                if(e->powerup != POWERUP_FIRE) {
                    v3 color = v3_Mul(powerup_colors[e->powerup], light);
                    RenderRect(e->pos, 0.3f, (v2) { e->rad, e->rad }, (v4) { color.r, color.g, color.b, 1.0f });
                }
            } break;
            case ENTITY_ENEMY: {
                RenderTexture(zombie_texture, (v3) { e->pos.x, e->pos.y, 0.3f }, e->rad, 0.1f * sinf(speed * platform.time_total), (v4) { light.r, light.g, light.b, 1.0f });
            } break;
            case ENTITY_CORPSE: {
                RenderTexture(zombie_texture, (v3) { e->pos.x, e->pos.y, 0.2f }, e->rad, 0.5f * PI, (v4) { 0.7f * light.r, 0.5f * light.g, 0.5f * light.b, 1.0f });
            } break;
            case ENTITY_HEALTH_PACK: {
                RenderTexture(health_pack_texture, (v3) { e->pos.x, e->pos.y, 0.2f }, 0.3f, 0.5f * PI, (v4) { light.r, light.g, light.b, 1.0f });
                
                const Entity* player = &em->array[0];
                v2 diff = v2_Sub(e->pos, player->pos);
                if(v2_Len(diff) > 10.0f) {
                    v2 dir = v2_Norm(diff);
                    RenderTexture(arrow_texture,
                                  (v3) { player->pos.x + dir.x * 3.0f, player->pos.y + dir.y * 3.0f, 1.0f },
                                  0.3f, v2_GetAngle((v2){0,1}, dir), (v4) { 1.0f, 1.0f, 1.0f, 0.3f });
                }
            } break;
        }
    }
}

static void GameInit(GameState* gs) {
    *gs = (GameState) {0};
    
    MapInit(&gs->map);
    
    v2 player_pos = GetValidSpawnLocation(&gs->map);
    
    gs->camera.current.xy = player_pos;
    
    EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_PLAYER, .pos = player_pos, .rad = 0.2f, .life = PLAYER_HEALTH, .powerup = POWERUP_NONE });
    
    zombies_killed = 0;
    health_pack_active = false;
    help_screen_bool = true;
}

static void HelpScreen(Camera* cam) {
    RenderStringFormat(cam->current.x - 9.0f, cam->current.y + 2.5f, cam->current.z - 8.0f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, " W");
    RenderStringFormat(cam->current.x - 9.0f, cam->current.y + 2.0f, cam->current.z - 8.0f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, "ASD");
    RenderStringFormat(cam->current.x - 7.0f, cam->current.y + 2.25f, cam->current.z - 8.0f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, "to move");
    RenderStringFormat(cam->current.x - 9.0f, cam->current.y + 0.0f, cam->current.z - 8.0f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, "SPACE to dash");
    RenderStringFormat(cam->current.x - 9.0f, cam->current.y - 2.0f, cam->current.z - 8.0f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, "LEFT MOUSE BUTTON to shoot");
    
}

static void GameRun(GameState* gs) {
    b32 game_running = true;
    
    GameInit(gs);
    
    f32 enemy_spawn_cooldown = 6.0f;
    
    while (game_running && !platform.close) {
        
        f32 dt = platform.time_delta;
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE]) {
            game_running    = false;
        }
        
        if (platform.key_pressed[GLFW_KEY_R]) {
            enemy_spawn_cooldown = 6.0f;
            GameInit(gs);
        }
        
        {
            EntityManager* em = &gs->entity_manager;
            
            if (em->array[0].type == ENTITY_PLAYER && (enemy_spawn_cooldown -= dt) <= 0.0f) {
                enemy_spawn_cooldown = 12.0f;
                
                v2 player_pos = em->array[0].pos;
                
                for (int i = 0; i < 64; ++i) {
                    v2 enemy_pos = GetValidSpawnLocation(&gs->map);
                    
                    while (v2_DistSq(player_pos, enemy_pos) < 12.0f * 12.0f)
                        enemy_pos = GetValidSpawnLocation(&gs->map);
                    
                    EntityAdd(em, &(Entity) { .type = ENTITY_ENEMY, .pos = enemy_pos, .rad = 0.2f, .life = 2.0f });
                }
            }
        }
        
        MapUpdate(&gs->map, &gs->particle_system, dt);
        
        AddLight(&gs->map, mouse_world_position.x, mouse_world_position.y, (v3) { 0.5f, 0.0f, 0.5f });
        
        HandleCollision(gs, dt);
        
        UpdateEntities(gs, dt);
        
        ParticlesUpdate(gs, dt);
        CameraUpdate(&gs->camera, dt);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // camera:
        {
            Camera* cam = &gs->camera;
            
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            
            gluPerspective(80.0f, platform.aspect, 0.1f, cam->current.z + 8.0f);
            gluLookAt(
                      cam->current.x, cam->current.y, cam->current.z,
                      cam->current.x, cam->current.y, 0.0f,
                      0.0f, 1.0f, 0.0f);
            
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }
        
        MapRender(&gs->map, &gs->camera);
        
        mouse_world_position = ToWorldPosition(
                                               iClamp(platform.mouse_position.x, 0, platform.width - 1),
                                               iClamp(platform.mouse_position.y, 1, platform.height));
        
        RenderEntities(&gs->entity_manager, &gs->map);
        ParticlesRender(&gs->particle_system, &gs->map);
        
        {
            Camera* cam = &gs->camera;
            
            glDisable(GL_DEPTH_TEST);
            
            RenderStringFormat(cam->current.x - 9.0f, cam->current.y + 6.0f, cam->current.z - 8.0f, 0.2f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f, "ms: %f", 1000.0f * dt);
            RenderStringFormat(cam->current.x + 7.0f, cam->current.y + 6.0f, cam->current.z - 8.0f, 0.2f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f, "Kills: %d", zombies_killed);
            RenderStringFormat(cam->current.x - 2.0f * 0.4f, cam->current.y + 6.0f, cam->current.z - 8.0f, 0.4f, -0.6f, 1.0f, 1.0f, 1.0f, 1.0f, "%.2f", powerup_switch_cooldown);
            
            int name_len = strlen(powerup_name[player_powerup]);
            
            RenderStringFormat(cam->current.x - 0.3f * 0.5f * name_len, cam->current.y + 5.0f, cam->current.z - 8.0f, 0.3f, -0.3f, 1.0f, 1.0f, 1.0f, 1.0f, powerup_name[player_powerup]);
            
            RenderTexture(aim_texture, (v3) { .xy = mouse_world_position.xy }, 0.15f, 0.5f * PI, (v4) { 1.0f, 1.0f, 1.0f, 1.0f });
            
            RenderRect(
                       (v2) { cam->current.x + 0.0f, cam->current.y - 6.0f }, cam->current.z - 8.0f,
                       (v2) { 4.0f, 0.05f }, (v4) { 1.0f, 0.0f, .0f, 1.0f });
            
            RenderRect(
                       (v2) { cam->current.x + 0.0f, cam->current.y - 6.0f }, cam->current.z - 8.0f,
                       (v2) { (player_health / PLAYER_HEALTH) * 4.0f, 0.05f }, (v4) { 0.0f, 1.0f, .0f, 1.0f });
            
            if(help_screen_bool) HelpScreen(cam);
            
            glEnable(GL_DEPTH_TEST);
        }
        
        AudioUpdate();
        PlatformUpdate();
    }
}

