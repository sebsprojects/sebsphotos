#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "platform.h"


static struct timespec tspec;

// ---------------------------------------------------------------------------

void glfwErrorCallback(i32 error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void createGlfw(GLFWwindow **window)
{
  glfwSetErrorCallback(glfwErrorCallback);
  if(!glfwInit()) {
    *window = 0;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_RESIZABLE, 0);
  GLFWwindow* w = glfwCreateWindow(640, 480, "Initializing", 0, 0);
  if(!w) {
    *window = 0;
  }
  glfwMakeContextCurrent(w);
  glfwSwapInterval(1);
  *window = w;
}

// ---------------------------------------------------------------------------

Platform *createPlatform()
{
  Platform *p = malloc(sizeof(Platform));
  p->startTime = getCurrentTime();
  createGlfw(&p->window);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glViewport(0, 0, 640, 480);
  return p;
}

void destroyPlatform(Platform *p)
{
  if(p->window != 0) {
    glfwDestroyWindow(p->window);
  }
  glfwTerminate();
  free(p);
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
  return getCurrentTime() - p->startTime;
}

