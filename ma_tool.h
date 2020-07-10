#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

typedef float   f32;
typedef double  f64;

typedef char        i8;
typedef short       i16;
typedef int         i32;
typedef long long   i64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef unsigned char       b8;
typedef unsigned short      b16;
typedef unsigned int        b32;
typedef unsigned long long  b64;

#ifndef false 
#define false   (0)
#endif

#ifndef true 
#define true   (!false)
#endif

#define ARRAY_COUNT(array) (sizeof (array) / sizeof (array[0]))

struct {
    b32             close;

    i32             width;
    i32             height;
    f32             aspect;

    GLFWwindow*     window;

    f32             time_delta;
    f32             time_total;

    u8              key_down        [GLFW_KEY_LAST + 1];
    u8              key_pressed     [GLFW_KEY_LAST + 1];
    u8              key_released    [GLFW_KEY_LAST + 1];
    
    u8              mouse_down      [GLFW_MOUSE_BUTTON_LAST + 1];
    u8              mouse_pressed   [GLFW_MOUSE_BUTTON_LAST + 1];
    u8              mouse_released  [GLFW_MOUSE_BUTTON_LAST + 1];
} platform;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    switch (action) {
        case GLFW_PRESS: {
            platform.key_down[key]      = true;
            platform.key_pressed[key]   = true;
        } break;
        case GLFW_RELEASE: {
            platform.key_down[key]      = false;
            platform.key_released[key]  = true;
        } break;
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;

    switch (action) {
        case GLFW_PRESS: {
            platform.mouse_down[button]      = true;
            platform.mouse_pressed[button]   = true;
        } break;
        case GLFW_RELEASE: {
            platform.mouse_down[button]      = false;
            platform.mouse_released[button]  = true;
        } break;
    }
}

void PlatformInit(const char* title, i32 width, i32 height) {
    glfwInit();

    platform.window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(platform.window);

    glfwSetKeyCallback(platform.window, KeyCallback);
    glfwSetMouseButtonCallback(platform.window, MouseButtonCallback);

    glfwSetTime(0.0f);
}

void PlatformUpdate(void) {
    platform.time_total += platform.time_delta = glfwGetTime(); glfwSetTime(0.0f);

    glfwGetWindowSize(platform.window, &platform.width, &platform.height);
    platform.aspect = (f32)platform.width / (f32)platform.height;

    glViewport(0, 0, platform.width, platform.height);

    for (int i = 0; i < ARRAY_COUNT(platform.key_pressed);  ++i) platform.key_pressed[i]  = false;
    for (int i = 0; i < ARRAY_COUNT(platform.key_released); ++i) platform.key_released[i] = false;

    for (int i = 0; i < ARRAY_COUNT(platform.mouse_pressed);  ++i) platform.mouse_pressed[i]  = false;
    for (int i = 0; i < ARRAY_COUNT(platform.mouse_released); ++i) platform.mouse_released[i] = false;

    if (glfwWindowShouldClose(platform.window)) {
        platform.close = true;
    }

    glfwSwapBuffers(platform.window);
    glfwPollEvents();
}

