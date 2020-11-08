#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "platform.h"
#include "texture.h"


i32 main(int argc, char **argv)
{
  char *shaderDir = "shader";
  Platform *platform = createPlatform(shaderDir);
  // TEMP
  char *testFile= "test/test_3.jpg";
  Texture *tex = allocTextureFromJPG(testFile);
  initTexture(tex, GL_NEAREST, GL_LINEAR);
  updateImgTex(platform->ws, tex, testFile);
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
