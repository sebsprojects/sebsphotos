#ifndef SEBSPHOTOS_PLATFORM
#define SEBSPHOTOS_PLATFORM

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
#include <math.h>

#include "common.h"
#include "jpg.h"


struct Platform {
  GLFWwindow *window;
  f64 startTime;
};
typedef struct Platform Platform;

Platform *createPlatform();
void destroyPlatform(Platform *p);

void printOpenGLInfo();

f64 getCurrentTime();
f64 getDiffToStartTime(Platform *p);

#endif
