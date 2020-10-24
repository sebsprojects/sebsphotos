#define GLFW_INCLUDE_ES2

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "platform.h"
#include "scenewim.h"
#include "texture.h"


Platform *platform = 0;
SceneWim *wim = 0;

// ---------------------------------------------------------------------------

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);
  drawSceneWim(wim);
}

i32 main(int argc, char **argv)
{
  char *shaderDir = "shader";
  platform = createPlatform();
  wim = createSceneWim(shaderDir);
  Texture *tex = allocTextureFromJPG("test/test_2.jpg");
  initTexture(tex);
  updateImgTex(wim, tex);
  updateImgGeom(wim, -0.95, -0.95, 0.95, 0.95);
  updateImgTexCoord(wim, 0, 0, tex->imgWidth, tex->imgHeight);
  while (!glfwWindowShouldClose(platform->window)) {
    render();
    glfwSwapBuffers(platform->window);
    glfwPollEvents();
  }
  if(platform != 0) { destroyPlatform(platform); }
  if(wim != 0) { destroySceneWim(wim); }
  if(tex != 0) { destroyTexture(tex); }
  return 0;
}
