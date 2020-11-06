#ifndef SEBSPHOTOS_TEXT
#define SEBSPHOTOS_TEXT

#include "shader.h"
#include "common.h"


struct Text {
  char *text;
  u32 textLen;
  VBO geom;
  VBO texC;
  IBO indices;
  Uniformf res;
  ShaderProgram textShader;
};
typedef struct Text Text;

Text *createText();
void destroyText(Text *t);

void updateText(Text *t, char *newText);
void updatePos(Text *t, f32 x, f32 y);

#endif
