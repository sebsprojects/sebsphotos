#define GLFW_INCLUDE_ES2

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "platform.h"
#include "scenewim.h"


Platform *platform = 0;
SceneWim *wim = 0;

// ---------------------------------------------------------------------------

void render()
{
  glClear(GL_COLOR_BUFFER_BIT);
  //glViewport(0, 0, (GLuint) scene->resolution[0],
  //                 (GLuint) scene->resolution[1]);
  //glActiveTexture(GL_TEXTURE0);
  //glBindTexture(GL_TEXTURE_2D, platform->texObject);

  //glBindBuffer(GL_ARRAY_BUFFER, platform->vertexBuffer);
  //glEnableVertexAttribArray(0);
  //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

  //glBindBuffer(GL_ARRAY_BUFFER, platform->textureBuffer);
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platform->indexBuffer);

  //drawScene(platform, scene);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

i32 main(int argc, char **argv)
{
  char *shaderDir = "shader";
  platform = createPlatform();
  wim = createSceneWim(shaderDir);
  while (!glfwWindowShouldClose(platform->window)) {
    render();
    glfwSwapBuffers(platform->window);
    glfwPollEvents();
  }
  if(platform != 0) {
    destroyPlatform(platform);
  }
  if(wim != 0) {
    destroySceneWim(wim);
  }
  return 0;
}
