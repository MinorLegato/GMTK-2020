
static void CreateBullet(EntityManager* em, Entity* e, v2 aim) {
    Entity p = {0};
    p.type = ENTITY_BULLET;
    p.pos = v2_Add(e->pos, v2_Scale(aim, 0.1f));
    p.rad = 0.1f;
    p.life = 1.0f;
    p.powerup = e->powerup;
    f32 speed = 5.0f;
    if(p.powerup == POWERUP_SPEED) speed = 10.0f;
    if(p.powerup == POWERUP_SLOW)  speed = 2.5f;
    p.vel = v2_Scale(aim, speed);
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
                    e->aim = v2_Norm(v2_Sub(mouse_world_position.xy, e->pos));
                    e->cooldown = powerup_cooldowns[e->powerup];
                    shoot = true;
                }
                
                EntityFriction(e, 4.0f);
                EntityApply(e, v2_Scale(v2_Norm(acc), 8.0f));
                
                AddLight(map, e->pos.x, e->pos.y, (v3) { 0.8f, 0.8f, 0.0f });
                
                gs->camera.target = (v3) {
                    .xy = v2_Add(e->pos, v2_Scale(e->vel, 0.8f)),
                    ._z = 12.0f + fClampMax(v2_Len(e->vel), 16.0f),
                };
            } break;
            case ENTITY_BULLET: {
                e->life -= dt;
            } break;
            case ENTITY_ENEMY: {
                AddLight(map, e->pos.x, e->pos.y, (v3) { 0.8f, 0.0f, 0.1f });
            } break;
        }
        
        EntityUpdate(e, dt);
        if(shoot) CreateBullet(em, e, e->aim);
        if(e->life <= 0.0f) EntityRemove(em, i);
        e->cooldown -= dt;
        if(e->type != ENTITY_BULLET && powerup_switch_cooldown <= 0.0f) {
            e->powerup = iRand(0, POWERUP_COUNT);
        }
    }
    if(powerup_switch_cooldown <= 0.0f) {
        powerup_switch_cooldown = 1.0f;
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

static void GameInit(GameState* gs) {
    *gs = (GameState) {0};
    
    EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_PLAYER, .pos = { 0.5f * MAP_WIDTH, 0.5f * MAP_HEIGHT }, .rad = 0.2f, .life = 1.0f, .powerup = POWERUP_NONE });
    
    for (int i = 0; i < 16; ++i) {
        EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_ENEMY, .pos = { fRand(0, MAP_WIDTH), fRand(0, MAP_HEIGHT) }, .rad = 0.2f, .life = 1.0f });
    }
    
    MapInit(&gs->map);
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

