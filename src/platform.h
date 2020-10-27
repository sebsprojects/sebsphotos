#ifndef SEBSPHOTOS_PLATFORM
#define SEBSPHOTOS_PLATFORM

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "common.h"
#include "scenewim.h"


struct Platform {
  GLFWwindow *window;
  i32 winWidth;
  i32 winHeight;
  SceneWim *wim;
  f64 lastMousePos[2];
  bool mouseLeftPressed;
  bool sceneDragActive;
};
typedef struct Platform Platform;

Platform *createPlatform(char *shaderDir);
void destroyPlatform(Platform *p);

void updateViewport(Platform *p);
void initGL();

void render(Platform *p);

void printOpenGLInfo();

f64 getCurrentTime();
f64 getDiffToStartTime(Platform *p);

#endif
