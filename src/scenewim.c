#include "scenewim.h"

#include <stdlib.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *s = malloc(sizeof(SceneWim));
  s->imgTex = 0;
  s->scrollLevel = 0;
  s->activeTool = TOOL_NAV;
  s->dragActive = 0;
  setZeros(s->res, 2);
  setZeros(s->sceneCoord, 4);
  setZeros(s->scrollCenter, 2);
  setZeros(s->scrollOffs, 2);
  setZeros(s->scrollT, 2);
  setZeros(s->imgTexOffs, 2);
  setZeros(s->selOffs, 2);
  setZeros(s->selCoord, 4);
  s->selAspect[0] = 3.0;
  s->selAspect[1] = 2.0;
  s->selSize = 0.0;
  initStandardShader(&s->imgShader, shaderDir, "mainImage");
  initStandardShader(&s->selShader, shaderDir, "select");
  initStandardIBO(&s->indices);
  initStandardGeom(&s->imgGeom, &s->imgShader, "in_position");
  initStandardGeom(&s->selGeom, &s->selShader, "in_position");
  initStandardTexCoord(&s->imgTexCoord, &s->imgShader, "in_texcoord");
  initStandardUniformf(&s->imgTexDim, 2, &s->imgShader, "texDim");
  initStandardUniformf(&s->zoomCenter, 2, &s->imgShader, "zoomCenter");
  initStandardUniformf(&s->selInnerCoord, 4, &s->selShader, "selInnerCoord");
  return s;
}

void destroySceneWim(SceneWim *s)
{
  if(s->imgGeom.data != 0) { free(s->imgGeom.data); }
  if(s->selGeom.data != 0) { free(s->selGeom.data); }
  if(s->imgTexCoord.data != 0) { free(s->imgTexCoord.data); }
  if(s->indices.data != 0) { free(s->indices.data); }
  free(s);
}

void handleMousePress(SceneWim *s, f32 cx, f32 cy)
{
  if(s->activeTool == TOOL_NAV) {
    s->dragActive = isInBounds(cx, cy, s->sceneCoord) && !s->dragActive;
  }
  if(s->activeTool == TOOL_SEL) {
    s->dragActive = isInSelBounds(s, cx, cy) && !s->dragActive;
  }
}

void handleMouseRelease(SceneWim *s)
{
  s->dragActive = 0;
}

void handleMouseScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll)
{
  if(isInBounds(cx, cy, s->sceneCoord)) {
    if(s->activeTool == TOOL_NAV) {
      updateImgScroll(s, cx, cy, scroll);
    }
    if(s->activeTool == TOOL_SEL) {
      updateSelScroll(s, cx, cy, scroll);
    }
  }
}

void handleMouseMove(SceneWim *s, f32 cx, f32 cy, f32 dx, f32 dy)
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

void updateImgTex(SceneWim *s, Texture *tex)
{
  s->imgTex = tex;
  s->imgTex->samplerLoc = glGetUniformLocation(s->imgShader.prog, "tex");
  s->imgTexDim.vec[0] = tex->texWidth;
  s->imgTexDim.vec[1] = tex->texHeight;
  s->selSize = 300.0;
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateSceneDimensions(SceneWim *s,
                           f32 w, f32 h, f32 x0, f32 y0, f32 x1, f32 y1)
{
  s->res[0] = w; s->res[1] = h;
  s->sceneCoord[0] = x0; s->sceneCoord[1] = y0;
  s->sceneCoord[2] = x1; s->sceneCoord[3] = y1;
  updateGeomData(&s->imgGeom, 1, x0 / w,
                                 y0 / h,
                                 x1 / w,
                                 y1 / h);
  //scaleToFit(s);
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateImgScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll)
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

  s->zoomCenter.vec[0] = s->scrollCenter[0] + s->imgTexOffs[0];
  s->zoomCenter.vec[1] = s->scrollCenter[1] + s->imgTexOffs[1];
  s->scrollLevel -= scroll;
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateImgDrag(SceneWim *s, f32 dx, f32 dy)
{
  if(s->imgTex == 0) { return; }
  s->imgTexOffs[0] += dx * scrollToScale(s->scrollLevel);
  s->imgTexOffs[1] += dy * scrollToScale(s->scrollLevel);
  calculateTexCoord(s);
  calculateSelCoord(s);
}

void updateSelScroll(SceneWim *S, f32 cx, f32 cy, f32 scroll)
{
}

void updateSelDrag(SceneWim *s, f32 dx, f32 dy)
{
  s->selOffs[0] -= dx * scrollToScale(s->scrollLevel);
  s->selOffs[1] -= dy * scrollToScale(s->scrollLevel);
  calculateSelCoord(s);
}

void setActiveTool(SceneWim *s, enum Tool t)
{
  s->activeTool = t;
  s->dragActive = 0;
}

void scaleToFit(SceneWim *s)
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

bool isInSelBounds(SceneWim *s, f32 cx, f32 cy)
{
  cx -= s->sceneCoord[0];
  cy -= s->sceneCoord[1];
  return isInBounds(cx, cy, s->selCoord);
}

f32 scrollToScale(f32 scroll)
{
  return 1.0 + 0.05 * scroll;
}

f32 scaleToScroll(f32 scale)
{
  return (scale - 1.0) * 20.0;
}

void calculateTexCoord(SceneWim *s)
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
  updateTexCoordData(&s->imgTexCoord,
                     offsX * zoom + tx,
                     offsY * zoom + ty,
                     (offsX + w) * zoom + tx,
                     (offsY + h) * zoom + ty);
}

void clipToScene(SceneWim *s, f32 *x0, f32 *y0, f32 *x1, f32 *y1)
{
  f32 w = s->sceneCoord[2] - s->sceneCoord[0];
  f32 h = s->sceneCoord[3] - s->sceneCoord[1];
  *x0 = maxf(0, *x0);
  *y0 = maxf(0, *y0);
  *x1 = minf(w, *x1);
  *y1 = minf(h, *y1);
}

void calculateSelCoord(SceneWim *s)
{
  f32 sx = s->sceneCoord[0];
  f32 sy = s->sceneCoord[1];
  f32 sh = s->sceneCoord[3] - sy;

  f32 zoom = 1.0 / scrollToScale(s->scrollLevel);
  f32 x = s->selOffs[0];
  f32 y = s->selOffs[1];
  f32 w = s->selAspect[0] * s->selSize;
  f32 h = s->selAspect[1] * s->selSize;

  f32 x0 = zoom * (x - s->scrollT[0]) - s->scrollOffs[0] - s->imgTexOffs[0];
  f32 y0 = zoom * (y - s->scrollT[1]) - s->scrollOffs[1] - s->imgTexOffs[1];
  f32 x1 = zoom * (x + w - s->scrollT[0]) - s->scrollOffs[0] - s->imgTexOffs[0];
  f32 y1 = zoom * (y + h - s->scrollT[1]) - s->scrollOffs[1] - s->imgTexOffs[1];

  s->selCoord[0] = x0;
  s->selCoord[1] = y0;
  s->selCoord[2] = x1;
  s->selCoord[3] = y1;
  clipToScene(s, &x0, &y0, &x1, &y1);

  printf("%f %f %f %f\n", x0, y0, x1, y1);

  updateGeomData(&s->selGeom, 1, (x0 + sx) / s->res[0], (sh - y0 + sy) / s->res[1],
                                 (x1 + sx) / s->res[0], (sh - y1 + sy) / s->res[1]);
}


void drawSceneWim(SceneWim *s)
{
  drawMainImage(s);
  drawSelect(s);
}

void drawSelect(SceneWim *s)
{
  updateVBO(&s->selGeom);

  glUseProgram(s->selShader.prog);
  glBindBuffer(GL_ARRAY_BUFFER, s->selGeom.id);
  glVertexAttribPointer(s->selGeom.loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(s->selGeom.loc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->indices.id);
  glDrawElements(GL_TRIANGLES, s->indices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(s->selGeom.loc);
  glUseProgram(0);
}

void drawMainImage(SceneWim *s)
{
  updateVBO(&s->imgGeom);
  updateVBO(&s->imgTexCoord);

  glUseProgram(s->imgShader.prog);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
  glVertexAttribPointer(s->imgGeom.loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(s->imgGeom.loc);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgTexCoord.id);
  glVertexAttribPointer(s->imgTexCoord.loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(s->imgTexCoord.loc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if(s->imgTex != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s->imgTex->id);
    glUniform1i(s->imgTex->samplerLoc, 0); // Value is texture unit num
    loadUniformf(&s->imgTexDim);
  }
  loadUniformf(&s->zoomCenter);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->indices.id);
  glDrawElements(GL_TRIANGLES, s->indices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(s->imgTexCoord.loc);
  glDisableVertexAttribArray(s->imgGeom.loc);
  glUseProgram(0);
  // FPS print
  //f64 now = getCurrentTime();
  //printf("%.2f ms\n", (now - scene->lastDrawTime) * 1000.0);
  //scene->lastDrawTime = now;
}

