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

void updateScrollLevel(SceneWim *s, f32 cx, f32 cy, f32 scrollOffs)
{
  if(s->imgTex == 0 || !isInBounds(cx, cy, s->sceneCoord)) {
    return;
  }
  s->scrollLevel += scrollOffs;
  s->scrollCenter[0] = cx - s->sceneCoord[0];
  s->scrollCenter[1] = cy - s->sceneCoord[1];
  calculateTexCoord(s);
}

void updateDrag(SceneWim *s, f32 dx, f32 dy)
{
  toTexCoords(s, &dx, &dy);
  s->imgTexOffs[0] += dx;
  s->imgTexOffs[1] += dy;
  calculateTexCoord(s);
}

void toTexCoords(SceneWim *s, f32 *cx, f32 *cy)
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
  updateTexCoordData(&s->imgTexCoord, x, y, w + x, h + y);
  // Calculate "base coords"
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

