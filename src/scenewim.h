#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#include "common.h"
#include "shader.h"
#include "texture.h"


struct SceneWim {
  ShaderProgram imgShader;
  VBO imgGeom;
  VBO imgTexCoord;
  IBO imgIndices;
  Uniformf imgTexDim;
  Uniformf zoomCenter;
  Texture *imgTex;
  f32 res[2];
  f32 sceneDim[4];
  f32 imgDim[4];
  f32 scrollLevel;
  f32 texScrollDim[4];
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

void updateSceneDimensions(SceneWim *s, f32 w, f32 h, f32 x0, f32 y0,
                           f32 x1, f32 y1);
void updateScrollLevel(SceneWim *s, f32 cx, f32 cy, f32 scrollOffs);

void updateImgGeom(SceneWim *s, f32 x0, f32 y0, f32 x1, f32 y1);
void updateImgTexCoord(SceneWim *s, f32 u0, f32 v0, f32 u1, f32 v1);
void updateImgTex(SceneWim *s, Texture *tex);

void drawSceneWim(SceneWim *scene);

#endif
