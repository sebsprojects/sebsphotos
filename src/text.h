#ifndef SEBSPHOTOS_TEXT
#define SEBSPHOTOS_TEXT

#include "shader.h"
#include "common.h"


struct Text {
  char *t;
  VBO geom;
  IBO indices;
  ShaderProgram textShader;
};
typedef struct Text Text;


Text *createText();
void destroyText();

#endif
