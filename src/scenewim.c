#include "scenewim.h"

#include <stdlib.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *s = malloc(sizeof(SceneWim));
  s->imgTex = 0;
  s->scrollLevel = 0;
  s->activeTool = TOOL_NAV;
  setZeros(s->res, 2);
  setZeros(s->sceneCoord, 4);
  setZeros(s->scrollCenter, 2);
  setZeros(s->scrollOffs, 2);
  setZeros(s->imgTexOffs, 2);
  s->selCoord[0] = 0.0; s->selCoord[1] = 0.0;
  s->selCoord[2] = 1200; s->selCoord[3] = 900;
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

void updateImgTex(SceneWim *s, Texture *tex)
{
  s->imgTex = tex;
  s->imgTex->samplerLoc = glGetUniformLocation(s->imgShader.prog, "tex");
  s->imgTexDim.vec[0] = tex->texWidth;
  s->imgTexDim.vec[1] = tex->texHeight;
  calculateTexCoord(s);
}

void updateSceneDimensions(SceneWim *s,
                           f32 w, f32 h, f32 x0, f32 y0, f32 x1, f32 y1)
{
  s->res[0] = w; s->res[1] = h;
  s->sceneCoord[0] = x0; s->sceneCoord[1] = y0;
  s->sceneCoord[2] = x1; s->sceneCoord[3] = y1;
  updateGeomData(&s->selGeom, (2 * s->selCoord[0] / w) - 1.0,
                              (2 * s->selCoord[1] / h) - 1.0,
                              (2 * s->selCoord[2] / w) - 1.0,
                              (2 * s->selCoord[3] / h) - 1.0);
  updateGeomData(&s->imgGeom, (2 * x0 / w) - 1.0,
                              (2 * y0 / h) - 1.0,
                              (2 * x1 / w) - 1.0,
                              (2 * y1 / h) - 1.0);
  calculateTexCoord(s);
}

void updateScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll)
{
  if(s->imgTex == 0 ||
     !isInBounds(cx, cy, s->sceneCoord) ||
     s->activeTool != TOOL_NAV) {
    return;
  }
  f32 ocx = s->scrollCenter[0];
  f32 ocy = s->scrollCenter[1];
  // Reconstruct the transformation of the old zoom level
  f32 zoom = getScrollScaling(s);
  f32 tx = ocx * (1.0 - zoom);
  f32 ty = ocy * (1.0 - zoom);

  // Calculate new scroll center
  f32 scx = cx - s->sceneCoord[0];
  f32 scy = cy - s->sceneCoord[1];
  scaleToTex(s, &scx, &scy);
  s->scrollCenter[0] = (scx + s->scrollOffs[0]) * zoom + tx;
  s->scrollCenter[1] = (scy + s->scrollOffs[1]) * zoom + ty;

  s->scrollOffs[0] = s->scrollCenter[0] - scx;
  s->scrollOffs[1] = s->scrollCenter[1] - scy;
  //printf("%f %f :: %f %f\n",ocx - s->scrollCenter[0],ocy - s->scrollCenter[1],
  //                           s->scrollOffs[0], s->scrollOffs[1]);

  s->zoomCenter.vec[0] = s->scrollCenter[0] + s->imgTexOffs[0];
  s->zoomCenter.vec[1] = s->scrollCenter[1] + s->imgTexOffs[1];
  s->scrollLevel += scroll;
  calculateTexCoord(s);
}

void updateDrag(SceneWim *s, f32 dx, f32 dy)
{
  if(s->imgTex == 0 || s->activeTool != TOOL_NAV) {
    return;
  }
  scaleToTex(s, &dx, &dy);
  s->imgTexOffs[0] += dx * getScrollScaling(s);
  s->imgTexOffs[1] += dy * getScrollScaling(s);
  calculateTexCoord(s);
}

void setActiveTool(SceneWim *s, enum Tool t)
{
  s->activeTool = t;
}

void scaleToTex(SceneWim *s, f32 *cx, f32 *cy)
{
  f32 texW = s->imgTex->imgWidth;
  f32 texH = s->imgTex->imgHeight;
  f32 sceW = s->sceneCoord[2] - s->sceneCoord[0];
  f32 sceH = s->sceneCoord[3] - s->sceneCoord[1];
  f32 xAsp = sceW / texW;
  f32 yAsp = sceH / texH;
  f32 scaling = (xAsp >= yAsp ? yAsp : xAsp);
  *cx = *cx / scaling;
  *cy = *cy / scaling;
}

f32 getScrollScaling(SceneWim *s)
{
  return 1.0 + 0.05 * s->scrollLevel;
}

/*
 * Depends on
 * sceneCoord
 * scrollLevel / scrollCenter
 */
void calculateTexCoord(SceneWim *s)
{
  f32 texW = s->imgTex->imgWidth;
  f32 texH = s->imgTex->imgHeight;
  f32 sceW = s->sceneCoord[2] - s->sceneCoord[0];
  f32 sceH = s->sceneCoord[3] - s->sceneCoord[1];

  f32 xAsp = sceW / texW;
  f32 yAsp = sceH / texH;
  f32 scaling = 1.0 / (xAsp >= yAsp ? yAsp : xAsp);
  f32 w = texW * xAsp * scaling;
  f32 h = texH * yAsp * scaling;
  f32 texOffsX = s->imgTexOffs[0];
  f32 texOffsY = s->imgTexOffs[1];
  f32 offsX = texOffsX + s->scrollOffs[0];
  f32 offsY = texOffsY + s->scrollOffs[1];

  f32 zoom = getScrollScaling(s);
  f32 tx = (s->scrollCenter[0] + texOffsX) * (1.0 - zoom);
  f32 ty = (s->scrollCenter[1] + texOffsY) * (1.0 - zoom);

  updateTexCoordData(&s->imgTexCoord,
                     offsX * zoom + tx,
                     offsY * zoom + ty,
                     (offsX + w) * zoom + tx,
                     (offsY + h) * zoom + ty);
}

void drawSceneWim(SceneWim *s)
{
  drawMainImage(s);
  if(s->activeTool == TOOL_SELECT) { drawSelect(s); }
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

