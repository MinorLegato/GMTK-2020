
static void GameInit(GameState* gs) {
    gs->cam = (Camera) {
        .target =  (v3) { 0.0f, 0.0f, 16.0f }
    };

    AddEntity(&gs->em, &(Entity) {
        .type   = ENTITY_PLAYER,
        .rad    = 0.5f,
    });
}

static void GameRun(GameState* gs) {
    b32 game_running = true;

    GameInit(gs);

    while (game_running) {
        f32 dt = platform.time_delta;
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE])
            game_running = false;
        
        MapUpdate(&gs->map, dt);

        UpdateEntities(&gs->em, dt);
        
        CameraUpdate(&gs->cam, dt);

        glClear(GL_COLOR_BUFFER_BIT);
        
        // camera:
        {
            Camera* cam = &gs->cam;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            gluPerspective(60.0f, platform.aspect, 0.1f, cam->current.z + 32.0f);
            gluLookAt(
                    cam->current.x, cam->current.y, cam->current.z,
                    cam->current.x, cam->current.y, 0.0f,
                    0.0f, 1.0f, 0.0f);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
        }

        MapRender(&gs->map);
        RenderEntities(&gs->em);

        PlatformUpdate();
    }
}
