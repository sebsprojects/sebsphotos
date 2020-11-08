#ifndef SEBSPHOTOS_INFOPANEL
#define SEBSPHOTOS_INFOPANEL

#include "common.h"
#include "text.h"
#include "shader.h"


struct Workspace;

struct Infopanel {
  struct Workspace *workspace;
  ShaderProgram textShader;
  Uniformf textRes;
  f32 panelCoord[4];
  Text sebText;
  Text imgHeading;
  Text imgName;
  Text imgSize;
  Text selHeading;
  Text selSize;
  Text selAspect;
  Text selCoord;
};
typedef struct Infopanel Infopanel;


Infopanel *createInfopanel(char *shaderDir);
void destroyInfopanel(Infopanel *inf);

void updateInfopanelDimensions(Infopanel *inf, f32 w, f32 h, f32 x0, f32 y0,
                               f32 x1, f32 y1);

void updateInfoSel(Infopanel *inf, f32 *selAspect, f32 *selBounds);
void updateInfoImg(Infopanel *inf, char *fileName, i32 w, i32 h);

void drawInfopanel(Infopanel *inf);

#endif
