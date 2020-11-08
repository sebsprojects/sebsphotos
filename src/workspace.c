#include "workspace.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "platform.h"
#include "infopanel.h"


Workspace *createWorkspace(char *shaderDir)
{
  Workspace *s = malloc(sizeof(Workspace));
  s->infopanel = 0;
  s->imgTex = 0;
  s->scrollLevel = 0;
  s->activeTool = TOOL_NAV;
  s->dragActive = 0;
  s->selClipActive = 0;
  s->mouseEntered = 0;
  setZeros(s->res, 2);
  setZeros(s->sceneCoord, 4);
  setZeros(s->scrollCenter, 2);
  setZeros(s->scrollOffs, 2);
  setZeros(s->scrollT, 2);
  setZeros(s->imgTexOffs, 2);
  setZeros(s->selOffs, 2);
  setZeros(s->selBounds, 4);
  s->selAspect[0] = 3.0;
  s->selAspect[1] = 2.0;
  s->selSize = 0.0;
  initShader(&s->imgShader, shaderDir, "mainImage");
  initShader(&s->selShader, shaderDir, "select");
  initIBO(&s->indices, 6);
  updateIBOData(&s->indices, 6, 0, IBO_DATA_SQUARE);
  updateIBO(&s->indices);
  initVBO(&s->imgGeom, 4, 2, &s->imgShader, "in_position");
  initVBO(&s->selGeom, 4, 2, &s->selShader, "in_position");
  initVBO(&s->imgTexC, 4, 2, &s->imgShader, "in_texcoord");
  initUniformf(&s->imgTexDim, 4, &s->imgShader, "texDim");
  initUniformf(&s->imgZoomCenter, 2, &s->imgShader, "zoomCenter");
  initUniformf(&s->imgSelBounds, 4, &s->imgShader, "selBounds");
  initUniformf(&s->imgSelClip, 1, &s->imgShader, "selClip");
  initUniformf(&s->selCoord, 4, &s->selShader, "selCoord");
  s->imgSelClip.vec[0] = -1.0;
  initUniformf(&s->selTime, 1, &s->selShader, "time");
  return s;
}

void destroyWorkspace(Workspace *s)
{
  if(s->imgGeom.data != 0) { free(s->imgGeom.data); }
  if(s->selGeom.data != 0) { free(s->selGeom.data); }
  if(s->imgTexC.data != 0) { free(s->imgTexC.data); }
  if(s->indices.data != 0) { free(s->indices.data); }
  free(s);
}

void handleMousePress(Workspace *s, f32 cx, f32 cy)
{
  if(s->activeTool == TOOL_NAV) {
    s->dragActive = isInBounds(cx, cy, s->sceneCoord) && !s->dragActive;
  }
  if(s->activeTool == TOOL_SEL) {
    s->dragActive = isInSelBounds(s, cx, cy) && !s->dragActive;
  }
}

void handleMouseRelease(Workspace *s)
{
  s->dragActive = 0;
}

void handleMouseScroll(Workspace *s, f32 cx, f32 cy, f32 scroll)
{
  if(isInBounds(cx, cy, s->sceneCoord) && s->activeTool == TOOL_NAV) {
      updateImgScroll(s, cx, cy, scroll);
  }
  //if(isInSelBounds(s, cx, cy) && s->activeTool == TOOL_SEL) {
  if(isInBounds(cx, cy, s->sceneCoord) && s->activeTool == TOOL_SEL) {
    updateSelScroll(s, cx, cy, scroll);
  }
}

void handleMouseMove(Workspace *s, f32 cx, f32 cy, f32 dx, f32 dy)
{
  if(isInBounds(cx, cy, s->sceneCoord)) {
    if(s->dragActive && s->activeTool == TOOL_NAV) {
      updateImgDrag(s, dx, dy);
    }
    if(s->dragActive && s->activeTool == TOOL_SEL) {
      updateSelDrag(s, dx, dy);
    }
  } else {
    s->dragActive = 0;
  }
}

void updateImgTex(Workspace *s, Texture *tex, char *fileName)
{
  s->imgTex = tex;
  s->imgTex->samplerLoc = glGetUniformLocation(s->imgShader.prog, "tex");
  s->imgTexDim.vec[0] = tex->texWidth;
  s->imgTexDim.vec[1] = tex->texHeight;
  s->imgTexDim.vec[2] = tex->imgWidth;
  s->imgTexDim.vec[3] = tex->imgHeight;
  s->selSize = 300.0;
  s->selOffs[0] = 0; // 0.5 * s->selSize * s->selAspect[0];
  s->selOffs[1] = 0; // 0.5 * s->selSize * s->selAspect[1];
  calculateTexCoord(s);
  calculateSelCoord(s);
  updateInfoImg(s->infopanel, fileName, tex->imgWidth, tex->imgHeight);
}

void updateSceneDimensions(Workspace *s,
                           f32 w, f32 h, f32 x0, f32 y0, f32 x1, f32 y1)
{
  s->res[0] = w; s->res[1] = h;
  s->sceneCoord[0] = x0; s->sceneCoord[1] = y0;
  s->sceneCoord[2] = x1; s->sceneCoord[3] = y1;
  setGeomData(&s->imgGeom, 1, 0, x0 / w,
                                 y0 / h,
                                 x1 / w,
                                 y1 / h);
  scaleToFit(s);
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateImgScroll(Workspace *s, f32 cx, f32 cy, f32 scroll)
{
  if(s->imgTex == 0 ||
     !isInBounds(cx, cy, s->sceneCoord) ||
     s->activeTool != TOOL_NAV) {
    return;
  }
  f32 ocx = s->scrollCenter[0];
  f32 ocy = s->scrollCenter[1];
  // Reconstruct the transformation of the old zoom level
  f32 zoom = scrollToScale(s->scrollLevel);
  f32 tx = ocx * (1.0 - zoom);
  f32 ty = ocy * (1.0 - zoom);

  // Calculate new scroll center
  f32 scx = cx - s->sceneCoord[0];
  f32 scy = cy - s->sceneCoord[1];

  s->scrollCenter[0] = (scx + s->scrollOffs[0]) * zoom + tx;
  s->scrollCenter[1] = (scy + s->scrollOffs[1]) * zoom + ty;

  s->scrollOffs[0] = s->scrollCenter[0] - scx;
  s->scrollOffs[1] = s->scrollCenter[1] - scy;
  //printf("%f %f :: %f %f\n",ocx - s->scrollCenter[0],ocy - s->scrollCenter[1],
  //                           s->scrollOffs[0], s->scrollOffs[1]);

  s->imgZoomCenter.vec[0] = s->scrollCenter[0] + s->imgTexOffs[0];
  s->imgZoomCenter.vec[1] = s->scrollCenter[1] + s->imgTexOffs[1];
  s->scrollLevel -= scroll;
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateImgDrag(Workspace *s, f32 dx, f32 dy)
{
  if(s->imgTex == 0) { return; }
  s->imgTexOffs[0] += dx * scrollToScale(s->scrollLevel);
  s->imgTexOffs[1] += dy * scrollToScale(s->scrollLevel);
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateSelScroll(Workspace *s, f32 cx, f32 cy, f32 scroll)
{
  scroll *= 100;;
  s->selSize += scroll;
  s->selOffs[0] -= scroll;
  s->selOffs[1] -= scroll;
  calculateSelCoord(s);
}

void updateSelDrag(Workspace *s, f32 dx, f32 dy)
{
  s->selOffs[0] -= dx * scrollToScale(s->scrollLevel);
  s->selOffs[1] -= dy * scrollToScale(s->scrollLevel);
  calculateSelCoord(s);
}

void toggleImgSelClip(Workspace *s)
{
  s->selClipActive = !s->selClipActive;
  s->imgSelBounds.vec[0] = s->selOffs[0];
  s->imgSelBounds.vec[1] = s->selOffs[1];
  s->imgSelBounds.vec[2] = s->selOffs[0] + s->selSize * s->selAspect[0];
  s->imgSelBounds.vec[3] = s->selOffs[1] + s->selSize * s->selAspect[1];
  s->imgSelClip.vec[0] *= -1.0;
}

void setActiveTool(Workspace *s, enum Tool t)
{
  s->activeTool = t;
  s->dragActive = 0;
}

void scaleToFit(Workspace *s)
{
  f32 texW = s->imgTex->imgWidth;
  f32 texH = s->imgTex->imgHeight;
  f32 sceW = s->sceneCoord[2] - s->sceneCoord[0];
  f32 sceH = s->sceneCoord[3] - s->sceneCoord[1];
  f32 xAsp = sceW / texW;
  f32 yAsp = sceH / texH;
  f32 scale = 1.0 / (xAsp >= yAsp ? yAsp : xAsp);
  s->scrollLevel = scaleToScroll(scale);
  calculateTexCoord(s);
}

bool isInSelBounds(Workspace *s, f32 cx, f32 cy)
{
  cx -= s->sceneCoord[0];
  cy -= s->sceneCoord[1];
  return isInBounds(cx, cy, s->selBounds);
}

f32 scrollToScale(f32 scroll)
{
  return 1.0 + 0.05 * scroll;
}

f32 scaleToScroll(f32 scale)
{
  return (scale - 1.0) * 20.0;
}

void calculateTexCoord(Workspace *s)
{
  f32 w = s->sceneCoord[2] - s->sceneCoord[0];
  f32 h = s->sceneCoord[3] - s->sceneCoord[1];
  f32 texOffsX = s->imgTexOffs[0];
  f32 texOffsY = s->imgTexOffs[1];
  f32 offsX = texOffsX + s->scrollOffs[0];
  f32 offsY = texOffsY + s->scrollOffs[1];

  f32 zoom = scrollToScale(s->scrollLevel);
  f32 tx = (s->scrollCenter[0] + texOffsX) * (1.0 - zoom);
  f32 ty = (s->scrollCenter[1] + texOffsY) * (1.0 - zoom);

  s->scrollT[0] = tx;
  s->scrollT[1] = ty;
  setTexCData(&s->imgTexC, 0, offsX * zoom + tx,
                                  offsY * zoom + ty,
                                  (offsX + w) * zoom + tx,
                                  (offsY + h) * zoom + ty);
}

void applySceneClip(Workspace *s, f32 *x0, f32 *y0, f32 *x1, f32 *y1)
{
  f32 sx = s->sceneCoord[0];
  f32 sy = s->sceneCoord[1];
  f32 sw = s->sceneCoord[2] - s->sceneCoord[0];
  f32 sh = s->sceneCoord[3] - s->sceneCoord[1];
  f32 oldy1 = *y1;
  *x0 = maxf(0, *x0) + sx;
  *y1 = sh - (maxf(0, *y0) - sy);
  *x1 = minf(sw, *x1) + sx;
  *y0 = sh - (minf(sh, oldy1) - sy);
}

void calculateSelCoord(Workspace *s)
{
  f32 zoom = 1.0 / scrollToScale(s->scrollLevel);
  f32 x = s->selOffs[0];
  f32 y = s->selOffs[1];
  f32 w = s->selAspect[0] * s->selSize;
  f32 h = s->selAspect[1] * s->selSize;

  // Bounds in scene space
  f32 x0 = zoom * (x - s->scrollT[0]) - s->scrollOffs[0] - s->imgTexOffs[0];
  f32 y0 = zoom * (y - s->scrollT[1]) - s->scrollOffs[1] - s->imgTexOffs[1];
  f32 x1 = zoom * (x + w - s->scrollT[0]) - s->scrollOffs[0] - s->imgTexOffs[0];
  f32 y1 = zoom * (y + h - s->scrollT[1]) - s->scrollOffs[1] - s->imgTexOffs[1];
  x0 = roundf(x0); y0 = roundf(y0);
  x1 = roundf(x1); y1 = roundf(y1);
  s->selBounds[0] = x0; s->selBounds[1] = y0;
  s->selBounds[2] = x1; s->selBounds[3] = y1;

  // Bounds + ow px in clip space
  f32 ow = 0.0;
  f32 ox0 = x0 - ow; f32 oy0 = y0 - ow;
  f32 ox1 = x1 + ow; f32 oy1 = y1 + ow;
  applySceneClip(s, &ox0, &oy0, &ox1, &oy1);
  s->selCoord.vec[0] = ox0; s->selCoord.vec[1] = oy0;
  s->selCoord.vec[2] = ox1; s->selCoord.vec[3] = oy1;
  //printf("%f %f %f %f\n", ox0, oy0, ox1, oy1);

  // Bounds + buf px in clip space
  f32 buf = 5.0;
  f32 cx0 = x0 - buf; f32 cy0 = y0 - buf;
  f32 cx1 = x1 + buf; f32 cy1 = y1 + buf;
  applySceneClip(s, &cx0, &cy0, &cx1, &cy1);
  setGeomData(&s->selGeom, 1, 0, cx0 / s->res[0],
                                 cy0 / s->res[1],
                                 cx1 / s->res[0],
                                 cy1 / s->res[1]);
}

void drawWorkspace(Workspace *s)
{
  drawMainImage(s);
  drawSelect(s);
}

void drawSelect(Workspace *s)
{
  if(s->selClipActive) {
    return;
  }
  updateVBO(&s->selGeom);

  glUseProgram(s->selShader.prog);
  glBindBuffer(GL_ARRAY_BUFFER, s->selGeom.id);
  glVertexAttribPointer(s->selGeom.loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(s->selGeom.loc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  loadUniformf(&s->selCoord);
  s->selTime.vec[0] = (f32) getDiffToStartTime();
  loadUniformf(&s->selTime);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->indices.id);
  glDrawElements(GL_TRIANGLES, s->indices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(s->selGeom.loc);
  glUseProgram(0);
}

void drawMainImage(Workspace *s)
{
  if(s->imgTex == 0) {
    return;
  }
  updateVBO(&s->imgGeom);
  updateVBO(&s->imgTexC);

  glUseProgram(s->imgShader.prog);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
  glVertexAttribPointer(s->imgGeom.loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(s->imgGeom.loc);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgTexC.id);
  glVertexAttribPointer(s->imgTexC.loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(s->imgTexC.loc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, s->imgTex->id);
  glUniform1i(s->imgTex->samplerLoc, 0); // Value is texture unit num

  loadUniformf(&s->imgTexDim);
  loadUniformf(&s->imgZoomCenter);
  loadUniformf(&s->imgSelClip);
  loadUniformf(&s->imgSelBounds);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->indices.id);
  glDrawElements(GL_TRIANGLES, s->indices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(s->imgTexC.loc);
  glDisableVertexAttribArray(s->imgGeom.loc);
  glUseProgram(0);
}

