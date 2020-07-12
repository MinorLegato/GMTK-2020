#include "ma_tool.h"

static v3 mouse_world_position;

#include "player_texture.c"
#include "zombie_texture.c"
#include "gun_texture.c"
#include "shotgun_texture.c"
#include "knife_texture.c"
#include "usi_texture.c"
#include "launcher_texture.c"
#include "aim_texture.c"
#include "arrow_texture.c"
#include "health_pack_texture.c"
#include "flame_texture.c"

#include "audio.c"

#include "render.h"

static Texture player_texture;
static Texture zombie_texture;
static Texture gun_texture;
static Texture shotgun_texture;
static Texture knife_texture;
static Texture usi_texture;
static Texture flame_texture;
static Texture launcher_texture;
static Texture aim_texture;
static Texture arrow_texture;
static Texture health_pack_texture;

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

int main(void) {
    PlatformInit("GMTK - 2020", 1600, 900);
    AudioInit();
    
#if 0 // enable vsync
    glfwSwapInterval(0);
#endif
    
    glfwSetInputMode(platform.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    BitmapInit();
    
    health_pack_texture = TextureCreate(health_pack_pixel_data, HEALTH_PACK_FRAME_WIDTH,    HEALTH_PACK_FRAME_HEIGHT,   false);
    player_texture      = TextureCreate(player_pixel_data,      PLAYER_FRAME_WIDTH,         PLAYER_FRAME_HEIGHT,        false);
    zombie_texture      = TextureCreate(zombie_pixel_data,      ZOMBIE_FRAME_WIDTH,         ZOMBIE_FRAME_HEIGHT,        false);
    gun_texture         = TextureCreate(gun_pixel_data,         GUN_FRAME_WIDTH,            GUN_FRAME_HEIGHT,           false);
    shotgun_texture     = TextureCreate(shotgun_pixel_data,     SHOTGUN_FRAME_WIDTH,        SHOTGUN_FRAME_HEIGHT,       false);
    knife_texture       = TextureCreate(knife_pixel_data,       KNIFE_FRAME_WIDTH,          KNIFE_FRAME_HEIGHT,         false);
    usi_texture         = TextureCreate(usi_pixel_data,         USI_FRAME_WIDTH,            USI_FRAME_HEIGHT,           false);
    flame_texture       = TextureCreate(flame_pixel_data,       FLAME_FRAME_WIDTH,          FLAME_FRAME_HEIGHT,         false);
    launcher_texture    = TextureCreate(launcher_pixel_data,    LAUNCHER_FRAME_WIDTH,       LAUNCHER_FRAME_HEIGHT,      false);
    aim_texture         = TextureCreate(aim_pixel_data,         AIM_FRAME_WIDTH,            AIM_FRAME_HEIGHT,           false);
    arrow_texture       = TextureCreate(arrow_pixel_data,       ARROW_FRAME_WIDTH,          ARROW_FRAME_HEIGHT,         false);
    
    GameState* gs = calloc(1, sizeof (GameState));
    
    blood_texture = TextureCreateV4(gs->map.blood, BLOOD_MAP_WIDTH, BLOOD_MAP_HEIGHT, false);
    
    GameRun(gs);
}

