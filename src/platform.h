#ifndef SEBSPHOTOS_PLATFORM
#define SEBSPHOTOS_PLATFORM

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "common.h"
#include "workspace.h"
#include "infopanel.h"


struct Platform {
  GLFWwindow *window;
  i32 winWidth;
  i32 winHeight;
  Workspace *ws;
  Infopanel *inf;
  f64 lastMousePos[2];
};
typedef struct Platform Platform;

Platform *createPlatform(char *shaderDir);
void destroyPlatform(Platform *p);

void updateViewport(Platform *p);
void initGL();

void render(Platform *p);

void printOpenGLInfo();

f64 getCurrentTime();
f64 getDiffToStartTime();

#endif
