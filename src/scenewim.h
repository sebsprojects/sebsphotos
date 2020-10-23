#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#include "common.h"
#include "platform.h"
#include "shader.h"


struct SceneWim {
  ShaderProgram mainShader;
};
typedef struct SceneWim SceneWim;

SceneWim *createSceneWim(char *shaderDir);
void destroySceneWim(SceneWim *wim);

void drawSceneWim(SceneWim *wim);

char *allocFileContent(char *path);
char *allocFullShaderPath(char *baseDir, char *fileName);

#endif
