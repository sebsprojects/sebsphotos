#ifndef SEBSPHOTOS_SHADER
#define SEBSPHOTOS_SHADER

#include <GLES2/gl2.h>

#include "common.h"


enum ShaderError {
  SHADER_NOERROR = 0,
  SHADER_ERROR_COMPILE_VERT = 1,
  SHADER_ERROR_COMPILE_FRAG = 2,
  SHADER_ERROR_LINK = 3,
  SHADER_ERROR_READ_SOURCE = 4
};

struct ShaderProgram {
  GLuint vert;
  GLuint frag;
  GLuint prog;
  enum ShaderError error;
};
typedef struct ShaderProgram ShaderProgram;

void initShader(ShaderProgram *p, char* shaderDir, char *shaderName);

void compileAndLinkShader(ShaderProgram *p, char *vertSrc, char *fragSrc);

char *allocFileContent(char *path);
char *allocFullShaderPath(char *baseDir, char *fileName, char *ext);

#endif
