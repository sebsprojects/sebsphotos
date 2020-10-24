#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "platform.h"


static i32 minWidth = 500;
static i32 minHeight = 300;

static struct timespec tspec;

// ---------------------------------------------------------------------------

void windowErrorCallback(i32 error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void windowResizeCallback(GLFWwindow *w, i32 newWidth, i32 newHeight)
{
  Platform *p = (Platform *) glfwGetWindowUserPointer(w);
  if(p == 0) {
    printf("GLFW User Pointer is 0\n");
  }
  p->winWidth = newWidth;
  p->winHeight = newHeight;
  updateViewport(p);
  updateSceneDimensions(p->wim, newWidth, newHeight,
                        5.0, 5.0, newWidth - 200.0, newHeight - 5.0);
}

// ---------------------------------------------------------------------------

void createWindow(Platform *platform)
{
  glfwSetErrorCallback(windowErrorCallback);
  if(!glfwInit()) { platform->window = 0; }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_RESIZABLE, 1);
  platform->window = glfwCreateWindow(platform->winWidth, platform->winHeight,
                                      "sebsphotos", 0, 0);
  glfwSetWindowSizeLimits(platform->window, minWidth, minHeight,
                          GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwSetWindowUserPointer(platform->window, platform);
  // Callbacks
  glfwSetFramebufferSizeCallback(platform->window, windowResizeCallback);
  glfwMakeContextCurrent(platform->window);
  glfwSwapInterval(1);
}

Platform *createPlatform(char *shaderDir)
{
  Platform *p = malloc(sizeof(Platform));
  p->winWidth = minWidth;
  p->winHeight = minHeight;
  createWindow(p);
  initGL();
  p->wim = createSceneWim(shaderDir);
  return p;
}

void destroyPlatform(Platform *p)
{
  if(p->window != 0) { glfwDestroyWindow(p->window); }
  if(p->wim != 0) { destroySceneWim(p->wim); }
  glfwTerminate();
  free(p);
}

void initGL()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
}

void updateViewport(Platform *p)
{
  glViewport(0, 0, p->winWidth, p->winHeight);
}


void render(Platform *p)
{
  glClear(GL_COLOR_BUFFER_BIT);
  drawSceneWim(p->wim);
}

void printOpenGLInfo()
{
  i32 maxTextureSize = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  printf("Max texture size: %i\n", maxTextureSize);
  float viewPortDim[2];
  glGetFloatv(GL_MAX_VIEWPORT_DIMS, viewPortDim);
  printf("Max viewport size: %f x %f\n", viewPortDim[0], viewPortDim[1]);
}

f64 getCurrentTime()
{
  clock_gettime(CLOCK_REALTIME, &tspec);
  return tspec.tv_sec + (tspec.tv_nsec / 1.0e9);
}

f64 getDiffToStartTime(Platform *p)
{
  //return getCurrentTime() - p->startTime;
  return 0.0;
}

