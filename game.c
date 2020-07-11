
static void CreateBullet(EntityManager* em, Entity* e, v2 aim) {
    Entity p = {0};
    p.type = ENTITY_BULLET;
    p.pos = v2_Add(e->pos, v2_Scale(aim, 0.1f));
    p.rad = 0.1f;
    p.life = 1.0f;
    p.powerup = e->powerup;
    f32 speed = 5.0f;
    if(p.powerup == BULLET_SPEED) speed = 10.0f;
    if(p.powerup == BULLET_SLOW)  speed = 2.5f;
    p.vel = v2_Scale(aim, speed);
    EntityAdd(em, &p);
}

static void UpdateEntities(GameState* gs, f32 dt) {
    EntityManager*  em  = &gs->entity_manager;
    Map*            map = &gs->map;
    
    for (int i = 0; i < em->count; ++i) {
        Entity* e = &em->array[i];
        
        switch (e->type) {
            case ENTITY_PLAYER: {
                v2 acc = {0};
                
                if (platform.key_down[GLFW_KEY_W]) acc.y += 1.0f;
                if (platform.key_down[GLFW_KEY_S]) acc.y -= 1.0f;
                if (platform.key_down[GLFW_KEY_A]) acc.x -= 1.0f;
                if (platform.key_down[GLFW_KEY_D]) acc.x += 1.0f;
                
                v2 aim = { 0.0f, 0.0f };
                if (platform.key_pressed[GLFW_KEY_UP])    aim.y += 1.0f;
                if (platform.key_pressed[GLFW_KEY_DOWN])  aim.y -= 1.0f;
                if (platform.key_pressed[GLFW_KEY_LEFT])  aim.x -= 1.0f;
                if (platform.key_pressed[GLFW_KEY_RIGHT]) aim.x += 1.0f;
                if(v2_Len(aim) > 0.0f) {
                    CreateBullet(em, e, v2_Norm(aim));
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
                // DO EVIL:
            } break;
        }
        
        EntityUpdate(e, dt);
        if(e->life <= 0.0f) EntityRemove(em, i);
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
                
                RenderRect(e->pos, 0.1f, (v2) { e->rad, e->rad }, (v4) { 1.0f, 1.0f, 1.0f, 1.0f });
            } break;
        }
    }
}

static void GameInit(GameState* gs) {
    *gs = (GameState) {0};
    
    EntityAdd(&gs->entity_manager, &(Entity) { .type = ENTITY_PLAYER, .pos = { 0.5f * MAP_WIDTH, 0.5f * MAP_HEIGHT }, .rad = 0.2f, .life = 1.0f });
    
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

