#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#include "common.h"
#include "platform.h"
#include "shader.h"


struct GLDataContainer {
  GLuint id;
  GLenum type;
  u32 count;
  u32 byteSize;
  bool isModified;
  void *data;
};
typedef struct GLDataContainer GLDataContainer;

struct SceneWim {
  ShaderProgram imgShader;
  GLDataContainer imgGeom;
  GLDataContainer imgTexCoord;
  GLDataContainer imgIndices;
};
typedef struct SceneWim SceneWim;

SceneWim *createSceneWim(char *shaderDir);
void destroySceneWim(SceneWim *scene);

void updateImgGeom(SceneWim *s, f32 x0, f32 y0, f32 x1, f32 y1);
void updateImgTexCoord(SceneWim *s, f32 u0, f32 v0, f32 u1, f32 v1);

void drawSceneWim(SceneWim *scene);

void initImgShader(SceneWim *s, char *shaderDir);
void initImgIndices(SceneWim *s);
void initImgGeom(SceneWim *s);
void initImgTexCoord(SceneWim *s);

void createBuffers(SceneWim *s);

f32 *arrayf32_at(void *data, u32 index);
i32 *arrayi32_at(void *data, u32 index);
u16 *arrayu16_at(void *data, u32 index);

char *allocFileContent(char *path);
char *allocFullShaderPath(char *baseDir, char *fileName);

#endif
