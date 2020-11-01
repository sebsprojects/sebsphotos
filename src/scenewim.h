#ifndef SEBSPHOTOS_SCENEWIM
#define SEBSPHOTOS_SCENEWIM

#include "common.h"
#include "shader.h"
#include "texture.h"

enum Tool {
  TOOL_NAV = 0,
  TOOL_SEL = 1
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
  Uniformf selCoordUni;
  Texture *imgTex;
  f32 imgTexOffs[2];
  f32 res[2];
  f32 sceneCoord[4];
  f32 selOffs[2];
  f32 selAspect[2];
  f32 selSize;
  f32 selCoord[4];
  f32 scrollCenter[2];
  f32 scrollOffs[2];
  f32 scrollT[2];
  f32 scrollLevel;
  bool dragActive;
  bool mouseEntered;
  enum Tool activeTool;
};
typedef struct SceneWim SceneWim;

SceneWim *createSceneWim(char *shaderDir);
void destroySceneWim(SceneWim *scene);

void handleMousePress(SceneWim *s, f32 cx, f32 cy);
void handleMouseRelease(SceneWim *s);
void handleMouseMove(SceneWim *s, f32 cx, f32 cy, f32 dx, f32 dy);
void handleMouseScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll);

void updateImgTex(SceneWim *s, Texture *tex);
void updateSceneDimensions(SceneWim *s, f32 w, f32 h, f32 x0, f32 y0,
                           f32 x1, f32 y1);

void updateImgScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll);
void updateSelScroll(SceneWim *S, f32 cx, f32 cy, f32 scroll);

void updateImgDrag(SceneWim *s, f32 dx, f32 dy);
void updateSelDrag(SceneWim *s, f32 dx, f32 dy);

void setActiveTool(SceneWim *s, enum Tool t);

void calculateTexCoord(SceneWim *s);
void calculateSelCoord(SceneWim *s);

bool isInSelBounds(SceneWim *s, f32 cx, f32 cy);

void drawSceneWim(SceneWim *s);
void drawMainImage(SceneWim *s);
void drawSelect(SceneWim *s);

void scaleToFit(SceneWim *s);
f32 scrollToScale(f32 scroll);
f32 scaleToScroll(f32 scale);

#endif
