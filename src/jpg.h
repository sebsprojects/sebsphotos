#ifndef SEBSPHOTOS_JPG
#define SEBSPHOTOS_JPG

#include "common.h"


struct TextureContainer {
  i32 imgWidth;
  i32 imgHeight;
  i32 texWidth;
  i32 texHeight;
  u8 *texBuffer;
};
typedef struct TextureContainer TextureContainer;

TextureContainer *allocTextureFromJPG(char *filePath);

void freeTexture(TextureContainer *tc);


#endif
