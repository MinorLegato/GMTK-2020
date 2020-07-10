
// game stuff:

static void gameRun() {
    b32 game_running = true;
    while(game_running) {
        f32 t = platform.time_delta;
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE])
            game_running = false;
        
        MapUpdate();
        EntityUpdate();
        
        MapRender();
        EntityRender();
        
        
        glClear(GL_COLOR_BUFFER_BIT);
        PlatformUpdate();
    }
}