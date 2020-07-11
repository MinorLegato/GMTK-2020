#include "ma_tool.h"

static v3 mouse_world_position;

#include "player_texture.c"
#include "zombie_texture.c"
#include "gun_texture.c"
#include "knife_texture.c"

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

static void RenderMainMenu(void) {
    RenderRect((v2) { 0.0f, 0.0f }, 0.0f, (v2) { 1.0f, 1.0f }, (v4) { 1.0f, 0.0f, 0.0f, 1.0f });
}

int main(void) {
    PlatformInit("GMTK - 2020", 1200, 800);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BitmapInit();

    player_texture  = TextureCreate((unsigned char*)player_pixel_data,  PLAYER_FRAME_WIDTH, PLAYER_FRAME_HEIGHT, false);
    zombie_texture  = TextureCreate((unsigned char*)zombie_pixel_data,  ZOMBIE_FRAME_WIDTH, ZOMBIE_FRAME_HEIGHT, false);
    gun_texture     = TextureCreate((unsigned char*)gun_pixel_data,     GUN_FRAME_WIDTH,    GUN_FRAME_HEIGHT,    false);
    knife_texture   = TextureCreate((unsigned char*)knife_pixel_data,   KNIFE_FRAME_WIDTH,  KNIFE_FRAME_HEIGHT,  false);

    GameState* gs = calloc(1, sizeof (GameState));
    
    while (!platform.close) {
        if (platform.key_pressed[GLFW_KEY_ENTER])
            GameRun(gs);
        
        if (platform.key_pressed[GLFW_KEY_ESCAPE])
            platform.close = true;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        RenderMainMenu();

        RenderStringFormat(0.0f, 0.0f, -0.1f, 0.05f, -0.05f, 1.0f, 1.0f, 1.0f, 1.0f, "Hello, world!");
        
        PlatformUpdate();
    }
}

