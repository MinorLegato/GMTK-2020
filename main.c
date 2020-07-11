#include "ma_tool.h"

static v3 mouse_world_position;

#include "camera.h"
#include "map.h"
#include "entity.h"
#include "particle.h"
#include "game.h"

#include "render.c"
#include "camera.c"
#include "map.c"
#include "entity.c"
#include "particle.c"
#include "game.c"

static void RenderMainMenu() {
    RenderRect((v2) { 0.0f, 0.0f }, 0.0f, (v2) { 1.0f, 1.0f }, (v4) { 1.0f, 0.0f, 0.0f, 1.0f });
}

int main(void) {
    PlatformInit("GMTK - 2020", 1200, 800);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    GameState* gs = calloc(1, sizeof (GameState));
    
    while (!platform.close) {
        if (platform.key_pressed[GLFW_KEY_ENTER])
            GameRun(gs);
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE])
            platform.close = true;
        
        RenderMainMenu();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        PlatformUpdate();
    }
}

