#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#include "common.h"
#include "shader.h"
#include "texture.h"

struct Platform;

struct SceneWim {
  ShaderProgram imgShader;
  VBO imgGeom;
  VBO imgTexCoord;
  IBO imgIndices;
  Uniformf imgTexDim;
  Uniformf zoomCenter;
  Texture *imgTex;
  f32 imgTexOffs[2];   // coords re to tex
  f32 res[2];
  f32 sceneCoord[4];
  f32 scrollLevel;
  f32 scrollCenter[2]; // coords rel to tex
};
typedef struct SceneWim SceneWim;

SceneWim *createSceneWim(char *shaderDir);
void destroySceneWim(SceneWim *scene);


void updateImgTex(SceneWim *s, Texture *tex);
void updateSceneDimensions(SceneWim *s, f32 w, f32 h, f32 x0, f32 y0,
                           f32 x1, f32 y1);


void updateDrag(SceneWim *s, f32 dx, f32 dy);

void calculateTexCoord(SceneWim *s);

void drawSceneWim(SceneWim *scene);

void toTexCoords(SceneWim *s, f32 *cx, f32 *cy);

#endif
