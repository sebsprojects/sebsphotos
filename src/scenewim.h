#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include "common.h"
#include "shader.h"
#include "texture.h"


// always f32
struct VBO {
  GLuint id;
  GLuint loc;
  bool isModified;
  u32 count;
  u32 dim;
  u32 byteSize;
  f32 *data;
};
typedef struct VBO VBO;

// always u16
struct IBO {
  GLuint id;
  u32 count;
  u32 byteSize;
  u16 *data;
};
typedef struct IBO IBO;

struct Uniformf {
  GLuint loc;
  u32 dim;
  f32 vec[4];
};
typedef struct Uniformf Uniformf;

struct SceneWim {
  ShaderProgram imgShader;
  VBO imgGeom;
  VBO imgTexCoord;
  IBO imgIndices;
  Uniformf imgTexDim;
  Texture *imgTex;
};
typedef struct SceneWim SceneWim;

SceneWim *createSceneWim(char *shaderDir);
void destroySceneWim(SceneWim *scene);

void initImgShader(SceneWim *s, char *shaderDir);
void initUniforms(SceneWim *s);
void initImgIndices(SceneWim *s);
void initImgGeom(SceneWim *s);
void initImgTexCoord(SceneWim *s);

void createBuffers(SceneWim *s);

void updateSceneDimensions(SceneWim *s, f32 w, f32 h,
                           f32 x0, f32 y0, f32 x1, f32 x2);

void updateImgGeom(SceneWim *s, f32 x0, f32 y0, f32 x1, f32 y1);
void updateImgTexCoord(SceneWim *s, f32 u0, f32 v0, f32 u1, f32 v1);
void updateImgTex(SceneWim *s, Texture *tex);

void drawSceneWim(SceneWim *scene);

void loadUniformf(Uniformf *u);

char *allocFileContent(char *path);
char *allocFullShaderPath(char *baseDir, char *fileName);

#endif
