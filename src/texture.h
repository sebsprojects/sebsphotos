#ifndef SEBSPHOTOS_TEXTURE
#define SEBSPHOTOS_TEXTURE

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "common.h"


struct Texture {
  i32 imgWidth;
  i32 imgHeight;
  u8 *imgBuffer;
  i32 texWidth;
  i32 texHeight;
  GLuint id;
  GLuint samplerLoc;
};
typedef struct Texture Texture;

Texture *allocTextureFromJPG(char *filePath);
void initTexture(Texture *t);

void destroyTexture(Texture *tc);


#endif
