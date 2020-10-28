#include "scenewim.h"

#include <stdlib.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *s = malloc(sizeof(SceneWim));
  s->imgTex = 0;
  s->scrollLevel = 0;
  setZeros(s->res, 2);
  setZeros(s->sceneCoord, 4);
  setZeros(s->scrollCenter, 2);
  setZeros(s->scrollOffs, 2);
  setZeros(s->imgTexOffs, 2);
  initStandardShader(&s->imgShader, shaderDir, "mainImage");
  initStandardIBO(&s->imgIndices);
  initStandardGeom(&s->imgGeom, &s->imgShader, "in_position");
  initStandardTexCoord(&s->imgTexCoord, &s->imgShader, "in_texcoord");
  initStandardUniformf(&s->imgTexDim, 2, &s->imgShader, "texDim");
  initStandardUniformf(&s->zoomCenter, 2, &s->imgShader, "zoomCenter");
  return s;
}

void destroySceneWim(SceneWim *s)
{
  if(s->imgShader.vertSrc != 0) { free(s->imgShader.vertSrc); }
  if(s->imgShader.fragSrc != 0) { free(s->imgShader.fragSrc); }
  if(s->imgGeom.data != 0) { free(s->imgGeom.data); }
  if(s->imgTexCoord.data != 0) { free(s->imgTexCoord.data); }
  if(s->imgIndices.data != 0) { free(s->imgIndices.data); }
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
  updateGeomData(&s->imgGeom, (2 * x0 / w) - 1.0,
                              (2 * y0 / h) - 1.0,
                              (2 * x1 / w) - 1.0,
                              (2 * y1 / h) - 1.0);
  calculateTexCoord(s);
}

void updateScroll(SceneWim *s, f32 cx, f32 cy, f32 scroll)
{
  if(s->imgTex == 0 || !isInBounds(cx, cy, s->sceneCoord)) {
    return;
  }
  f32 ocx = s->scrollCenter[0];
  f32 ocy = s->scrollCenter[1];
  // Reconstruct the transformation of the old zoom level
  f32 zoom = 1.0 + 0.05 * s->scrollLevel;
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
  printf("%f %f :: %f %f\n", ocx - s->scrollCenter[0],
                             ocy - s->scrollCenter[1],
                             s->scrollOffs[0],
                             s->scrollOffs[1]);

  s->zoomCenter.vec[0] = s->scrollCenter[0] + s->imgTexOffs[0];
  s->zoomCenter.vec[1] = s->scrollCenter[1] + s->imgTexOffs[1];
  s->scrollLevel += scroll;
  calculateTexCoord(s);
}

void updateDrag(SceneWim *s, f32 dx, f32 dy)
{
  scaleToTex(s, &dx, &dy);
  s->imgTexOffs[0] += dx;
  s->imgTexOffs[1] += dy;
  calculateTexCoord(s);
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
  f32 x = s->imgTexOffs[0];
  f32 y = s->imgTexOffs[1];

  f32 zoom = 1.0 + 0.05 * s->scrollLevel;
  f32 tx = (s->scrollCenter[0] + x) * (1.0 - zoom);
  f32 ty = (s->scrollCenter[1] + y) * (1.0 - zoom);

  updateTexCoordData(&s->imgTexCoord,
                     (x + s->scrollOffs[0]) * zoom + tx,
                     (y + s->scrollOffs[1]) * zoom + ty,
                     (w + x + s->scrollOffs[0]) * zoom + tx,
                     (h + y + s->scrollOffs[1]) * zoom + ty);
}

void drawSceneWim(SceneWim *s)
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

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->imgIndices.id);
  glDrawElements(GL_TRIANGLES, s->imgIndices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(s->imgTexCoord.loc);
  glDisableVertexAttribArray(s->imgGeom.loc);
  glUseProgram(0);
  // FPS print
  //f64 now = getCurrentTime();
  //printf("%.2f ms\n", (now - scene->lastDrawTime) * 1000.0);
  //scene->lastDrawTime = now;
}

