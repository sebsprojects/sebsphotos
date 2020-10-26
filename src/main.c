#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "platform.h"
#include "texture.h"


i32 main(int argc, char **argv)
{
  char *shaderDir = "shader";
  Platform *platform = createPlatform(shaderDir);
  // TEMP
  Texture *tex = allocTextureFromJPG("test/test_2.jpg");
  initTexture(tex);
  updateImgTex(platform->wim, tex);
  // Main loop
  while (!glfwWindowShouldClose(platform->window)) {
    render(platform);
    glfwSwapBuffers(platform->window);
    glfwPollEvents();
  }
  // Peaceful exit
  if(platform != 0) { destroyPlatform(platform); }
  if(tex != 0) { destroyTexture(tex); }
  return 0;
}
