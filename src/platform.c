#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "platform.h"
#include "text.h"


static i32 minWidth = 500;
static i32 minHeight = 300;

static struct timespec tspec;
static f64 startTime = 0.0;

static Text *t = 0;

// ---------------------------------------------------------------------------

void windowErrorCallback(i32 error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void windowResizeCallback(GLFWwindow *w, i32 newWidth, i32 newHeight)
{
  Platform *p = (Platform *) glfwGetWindowUserPointer(w);
  if(p == 0) {
    printf("GLFW User Pointer is NULL\n");
  }
  p->winWidth = newWidth;
  p->winHeight = newHeight;
  updateViewport(p);
  t->res.vec[0] = newWidth;
  t->res.vec[1] = newHeight;
  updateSceneDimensions(p->ws, newWidth, newHeight,
                        5.0, 5.0, newWidth - 200.0, newHeight - 5.0);
}

void windowScrollCallback(GLFWwindow *w, f64 xoffs, f64 yoffs)
{
  Platform *p = (Platform *) glfwGetWindowUserPointer(w);
  f64 cx = 0.0; f64 cy = 0.0;
  glfwGetCursorPos(w, &cx, &cy);
  handleMouseScroll(p->ws, cx, cy, yoffs);
}

void windowMouseButtonCallback(GLFWwindow *w, i32 button, i32 action, i32 mods)
{
  Platform *p = (Platform *) glfwGetWindowUserPointer(w);
  f64 cx = 0.0; f64 cy = 0.0;
  glfwGetCursorPos(w, &cx, &cy);
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    handleMousePress(p->ws, cx, cy);
  }
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    handleMouseRelease(p->ws);
  }
}

void windowMousePosCallback(GLFWwindow *w, f64 xpos, f64 ypos)
{
  Platform *p = (Platform *) glfwGetWindowUserPointer(w);
  if(p->lastMousePos[0] >= 0.0 && p->lastMousePos[1] >= 0.0) {
    f32 dx = (f32) p->lastMousePos[0] - xpos;
    f32 dy = (f32) p->lastMousePos[1] - ypos;
    handleMouseMove(p->ws, (f32) xpos, (f32) ypos, dx, dy);
  }
  p->lastMousePos[0] = xpos;
  p->lastMousePos[1] = ypos;
}

void windowKeyCallback(GLFWwindow *w, i32 key, i32 scode, i32 action, i32 mods)
{
  Platform *p = (Platform *) glfwGetWindowUserPointer(w);
  if(key == GLFW_KEY_1) {
    setActiveTool(p->ws, TOOL_NAV);
  }
  if(key == GLFW_KEY_2) {
    setActiveTool(p->ws, TOOL_SEL);
  }
  if(key == GLFW_KEY_C && action == GLFW_PRESS) {
    toggleImgSelClip(p->ws);
  }
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
  glfwSetScrollCallback(platform->window, windowScrollCallback);
  glfwSetMouseButtonCallback(platform->window, windowMouseButtonCallback);
  glfwSetCursorPosCallback(platform->window, windowMousePosCallback);
  glfwSetKeyCallback(platform->window, windowKeyCallback);
  glfwMakeContextCurrent(platform->window);
  glfwSwapInterval(1);
}


Platform *createPlatform(char *shaderDir)
{
  startTime = getCurrentTime();
  Platform *p = malloc(sizeof(Platform));
  p->winWidth = minWidth;
  p->winHeight = minHeight;
  p->lastMousePos[0] = -1.0;
  p->lastMousePos[1] = -1.0;
  createWindow(p);
  initGL();
  p->ws = createWorkspace(shaderDir);
  t = createText(shaderDir);
  updateText(t, "sebsphotos is great.,_Isn't it RIGHT?");
  return p;
}

void destroyPlatform(Platform *p)
{
  if(p->window != 0) { glfwDestroyWindow(p->window); }
  if(p->ws != 0) { destroyWorkspace(p->ws); }
  glfwTerminate();
  free(p);
}

void initGL()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void updateViewport(Platform *p)
{
  glViewport(0, 0, p->winWidth, p->winHeight);
}


void render(Platform *p)
{
  glClear(GL_COLOR_BUFFER_BIT);
  //drawWorkspace(p->ws);
  drawText(t);
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

f64 getDiffToStartTime()
{
  return getCurrentTime() - startTime;
}

