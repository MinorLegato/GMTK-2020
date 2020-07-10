#include "ma_tool.h"

int main(void) {
    PlatformInit("GMTK", 1200, 800);

    while (!platform.close) {
        f32 t = platform.time_delta;

        if (platform.key_pressed[GLFW_KEY_ESCAPE])
            platform.close = true;

        glClear(GL_COLOR_BUFFER_BIT);

        PlatformUpdate();
    }
}

