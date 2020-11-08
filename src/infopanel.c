#include "infopanel.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "workspace.h"


Infopanel *createInfopanel(char *shaderDir)
{
  Infopanel *inf = malloc(sizeof(Infopanel));
  inf->workspace = 0;
  setZeros(inf->panelCoord, 4);
  initShader(&inf->textShader, shaderDir, "text");
  initUniformf(&inf->textRes, 2, &inf->textShader, "res");
  initText(&inf->sebText, &inf->textShader, &inf->textRes);
  initText(&inf->imgHeading, &inf->textShader, &inf->textRes);
  initText(&inf->imgName, &inf->textShader, &inf->textRes);
  initText(&inf->imgSize, &inf->textShader, &inf->textRes);
  initText(&inf->selHeading, &inf->textShader, &inf->textRes);
  initText(&inf->selSize, &inf->textShader, &inf->textRes);
  initText(&inf->selAspect, &inf->textShader, &inf->textRes);
  initText(&inf->selCoord, &inf->textShader, &inf->textRes);
  updateText(&inf->sebText, "sebsphotos v?");
  updateText(&inf->imgHeading, "Image");
  updateInfoImg(inf, 0, -1, -1);
  updateInfoSel(inf, 0, 0);
  updateText(&inf->selHeading, "Select");
  return inf;
}

void destroyInfopanel(Infopanel *inf)
{
  destroyText(&inf->sebText);
  destroyText(&inf->imgHeading);
  destroyText(&inf->imgName);
  destroyText(&inf->imgSize);
  destroyText(&inf->selHeading);
  destroyText(&inf->selSize);
  destroyText(&inf->selAspect);
  destroyText(&inf->selCoord);
  free(inf);
}

void updateInfoImg(Infopanel *inf, char *fileName, i32 w, i32 h)
{
  //TODO: 100 is a good guess but may possibly fail
  if(fileName == 0 || w < 0.0 || h < 0.0) {
    updateText(&inf->imgName, "Name: unknown");
    updateText(&inf->imgSize, "Size: unknown");
  } else {
    char *buf = malloc(strlen(fileName) + 100);
    sprintf(buf, "Name: %s", fileName);
    updateText(&inf->imgName, buf);
    sprintf(buf, "Size: %ix%i", w, h);
    updateText(&inf->imgSize, buf);
    free(buf);
  }
}

void updateInfoSel(Infopanel *inf, f32 *selAspect, f32 *selBounds)
{
  if(selAspect == 0 || selBounds == 0) {
    updateText(&inf->selSize,   "Size:   unknown");
    updateText(&inf->selAspect, "Aspect: unknown");
    updateText(&inf->selCoord,  "Coord:  unknown");
  } else {
    char *buf = malloc(8 + 4 * 10 + 100 + 1);
    i32 w = (i32) (selBounds[2] - selBounds[0]);
    i32 h = (i32) (selBounds[3] - selBounds[1]);
    sprintf(buf, "Size:   %ix%i", w, h);
    updateText(&inf->selSize, buf);
    sprintf(buf, "Aspect: %.1g:%.1g", selAspect[0], selAspect[1]);
    updateText(&inf->selAspect, buf);
    sprintf(buf, "Coord:  %i %i :: %i %i", (i32) selBounds[0],
                                          (i32) selBounds[1],
                                          (i32) selBounds[2],
                                          (i32) selBounds[3]);
    updateText(&inf->selCoord, buf);
    free(buf);
  }
}

void updateInfopanelDimensions(Infopanel *inf, f32 w, f32 h, f32 x0, f32 y0,
                               f32 x1, f32 y1)
{
  inf->textRes.vec[0] = w; inf->textRes.vec[1] = h;
  inf->panelCoord[0] = x0; inf->panelCoord[1] = y0;
  inf->panelCoord[2] = x1; inf->panelCoord[3] = y1;
  f32 hsize = 16.0;
  f32 nsize = 14.0;
  f32 x = 5.0 + x0;
  f32 y = h;
  f32 step = nsize + 2.0;
  updateTextDimensions(&inf->sebText, x, y -= step, hsize);
  y -= step;
  updateTextDimensions(&inf->imgHeading, x, y -= step, hsize);
  y -= 2.0;
  updateTextDimensions(&inf->imgName, x, y -= step, nsize);
  updateTextDimensions(&inf->imgSize, x, y -= step, nsize);
  y -= step;
  updateTextDimensions(&inf->selHeading, x, y -= step, hsize);
  y -= 2.0;
  updateTextDimensions(&inf->selSize, x, y -= step, nsize);
  updateTextDimensions(&inf->selAspect, x, y -= step, nsize);
  updateTextDimensions(&inf->selCoord, x, y -= step, nsize);
}

void drawInfopanel(Infopanel *inf)
{
  drawText(&inf->sebText);
  drawText(&inf->imgHeading);
  drawText(&inf->imgName);
  drawText(&inf->imgSize);
  drawText(&inf->selHeading);
  drawText(&inf->selSize);
  drawText(&inf->selAspect);
  drawText(&inf->selCoord);
}
