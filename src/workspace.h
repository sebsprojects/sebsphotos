#ifndef SEBSPHOTOS_WORKSPACE
#define SEBSPHOTOS_WORKSPACE

#include "common.h"
#include "shader.h"
#include "texture.h"


struct Infopanel;

enum Tool {
  TOOL_NAV = 0,
  TOOL_SEL = 1
};

struct Workspace {
  struct Infopanel *infopanel;
  ShaderProgram imgShader;
  ShaderProgram selShader;
  VBO selGeom;
  VBO imgGeom;
  VBO imgTexC;
  IBO indices;
  Texture *imgTex;
  Uniformf imgTexDim;
  Uniformf imgZoomCenter;
  Uniformf imgSelBounds;
  Uniformf imgSelClip;
  Uniformf selCoord;
  Uniformf selTime;
  f32 imgTexOffs[2];
  f32 res[2];
  f32 sceneCoord[4];
  f32 selOffs[2];
  f32 selAspect[2];
  f32 selSize;
  f32 selBounds[4];
  f32 scrollCenter[2];
  f32 scrollOffs[2];
  f32 scrollT[2];
  f32 scrollLevel;
  bool dragActive;
  bool selClipActive;
  bool mouseEntered;
  enum Tool activeTool;
};
typedef struct Workspace Workspace;

Workspace *createWorkspace(char *shaderDir);
void destroyWorkspace(Workspace *scene);

void handleMousePress(Workspace *s, f32 cx, f32 cy);
void handleMouseRelease(Workspace *s);
void handleMouseMove(Workspace *s, f32 cx, f32 cy, f32 dx, f32 dy);
void handleMouseScroll(Workspace *s, f32 cx, f32 cy, f32 scroll);

void updateImgTex(Workspace *s, Texture *tex);
void updateSceneDimensions(Workspace *s, f32 w, f32 h, f32 x0, f32 y0,
                           f32 x1, f32 y1);

void updateImgScroll(Workspace *s, f32 cx, f32 cy, f32 scroll);
void updateSelScroll(Workspace *S, f32 cx, f32 cy, f32 scroll);

void updateImgDrag(Workspace *s, f32 dx, f32 dy);
void updateSelDrag(Workspace *s, f32 dx, f32 dy);

void setActiveTool(Workspace *s, enum Tool t);
void toggleImgSelClip(Workspace *s);

void calculateTexCoord(Workspace *s);
void calculateSelCoord(Workspace *s);

bool isInSelBounds(Workspace *s, f32 cx, f32 cy);

void drawWorkspace(Workspace *s);
void drawMainImage(Workspace *s);
void drawSelect(Workspace *s);

void scaleToFit(Workspace *s);
f32 scrollToScale(f32 scroll);
f32 scaleToScroll(f32 scale);

#endif
