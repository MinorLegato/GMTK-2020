#include <stdint.h>

#define ZOMBIE_FRAME_COUNT 1
#define ZOMBIE_FRAME_WIDTH 8
#define ZOMBIE_FRAME_HEIGHT 8

static const uint32_t zombie_pixel_data[1][64] = {
    {
        0x00000000, 0x00000000, 0x00000000, 0xff3a6d2d, 0xff3a6d2d, 0x00000000, 0x00000000, 0x00000000, 
        0x00000000, 0x00000000, 0x00000000, 0xff3a6d2d, 0xff3a6d2d, 0x00000000, 0x00000000, 0x00000000, 
        0x00000000, 0xff3a6d2d, 0xff576e78, 0xff3a6d2d, 0xff3a6d2d, 0xff576e78, 0xff3a6d2d, 0x00000000, 
        0x00000000, 0xff3a6d2d, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff3a6d2d, 0x00000000, 
        0xff3a6d2d, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff3a6d2d, 
        0xff3a6d2d, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff576e78, 0xff3a6d2d, 
        0x00000000, 0xff294049, 0xff294049, 0xff294049, 0xff294049, 0xff294049, 0xff294049, 0x00000000, 
        0x00000000, 0xff294049, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xff294049, 0x00000000
    }
};
