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
  Uniformf uDims;
  Uniformf *rel_res;
  ShaderProgram *rel_textShader;
};
typedef struct Text Text;

void initText(Text *t, ShaderProgram *rel_textShader, Uniformf *rel_res);
void destroyText(Text *t);

void updateText(Text *t, char *newText);
void updateTextDimensions(Text *t, f32 x, f32 y, f32 size);

void drawText(Text *t);

#endif
