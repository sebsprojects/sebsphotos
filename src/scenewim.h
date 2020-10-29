#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#include "common.h"
#include "shader.h"
#include "texture.h"

enum Tool {
  TOOL_NAV = 0,
  TOOL_SELECT = 1
};
struct SceneWim {
  ShaderProgram imgShader;
  ShaderProgram selShader;
  VBO selGeom;
  VBO imgGeom;
  VBO imgTexCoord;
  IBO indices;
  Uniformf imgTexDim;
  Uniformf zoomCenter;
  Uniformf selInnerCoord;
  Texture *imgTex;
  f32 imgTexOffs[2];   // coords re to tex
  f32 res[2];
  f32 sceneCoord[4];
  f32 selCoord[4];
  f32 scrollCenter[2];
  f32 scrollOffs[2];
  f32 scrollLevel;
  enum Tool activeTool;
};
typedef struct SceneWim SceneWim;

SceneWim *createSceneWim(char *shaderDir);
void destroySceneWim(SceneWim *scene);


void updateImgTex(SceneWim *s, Texture *tex);
void updateSceneDimensions(SceneWim *s, f32 w, f32 h, f32 x0, f32 y0,
                           f32 x1, f32 y1);


void updateScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll);
void updateDrag(SceneWim *s, f32 dx, f32 dy);

void setActiveTool(SceneWim *s, enum Tool t);

void calculateTexCoord(SceneWim *s);

void drawSceneWim(SceneWim *s);

void drawMainImage(SceneWim *s);
void drawSelect(SceneWim *s);

void scaleToTex(SceneWim *s, f32 *cx, f32 *cy);
f32 getScrollScaling(SceneWim *s);

#endif
