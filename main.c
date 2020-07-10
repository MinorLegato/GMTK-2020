#include "ma_tool.h"

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
    RenderRect((v2) { 0.0f, 0.0f }, (v2) { 1.0f, 1.0f }, (v4) { 255, 0, 0, 255 });
}

int main(void) {
    PlatformInit("GMTK", 1200, 800);

    GameState* gs = calloc(1, sizeof (GameState));
    
    while (!platform.close) {
        if (platform.key_pressed[GLFW_KEY_ENTER])
            GameRun(gs);
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE])
            platform.close = true;
        
        RenderMainMenu();

        glClear(GL_COLOR_BUFFER_BIT);
        
        PlatformUpdate();
    }
}
