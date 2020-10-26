#include "scenewim.h"

#include <stdlib.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *s = malloc(sizeof(SceneWim));
  s->imgTex = 0;
  s->scrollLevel = 0;
  setZeros(s->res, 2);
  setZeros(s->sceneDim, 4);
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
  updateTexCoordData(&s->imgTexCoord, 0.0, 0.0,
                                      s->imgTex->imgWidth,
                                      s->imgTex->imgHeight);
  s->texScrollDim[0] = 0.0;
  s->texScrollDim[1] = 0.0;
  s->texScrollDim[2] = s->imgTex->imgWidth;
  s->texScrollDim[3] = s->imgTex->imgHeight;
}

void updateSceneDimensions(SceneWim *s,
                           f32 w, f32 h, f32 x0, f32 y0, f32 x1, f32 y1)
{
  s->res[0] = w; s->res[1] = h;
  s->sceneDim[0] = x0; s->sceneDim[1] = y0;
  s->sceneDim[2] = x1; s->sceneDim[3] = y1;
  if(s->imgTex != 0) {
    f32 sw = x1 - x0;
    f32 sh = y1 - y0;
    f32 tw = s->imgTex->imgWidth;
    f32 th = s->imgTex->imgHeight;
    f32 wRatio = tw / sw;
    f32 hRatio = th / sh;
    f32 scaling = 1.0 / (wRatio >= hRatio ? wRatio : hRatio);
    f32 xpad = sw - scaling * tw;
    f32 ypad = sh - scaling * th;
    s->imgDim[0] = x0 + 0.5 * xpad; s->imgDim[1] = y0 + 0.5 * ypad;
    s->imgDim[2] = x1 - 0.5 * xpad; s->imgDim[3] = y1 - 0.5 * ypad;
    updateGeomData(&s->imgGeom, 2.0 * s->imgDim[0] / w - 1.0,
                                2.0 * s->imgDim[1] / h - 1.0,
                                2.0 * s->imgDim[2] / w - 1.0,
                                2.0 * s->imgDim[3] / h - 1.0);
  }
}

void updateScrollLevel(SceneWim *s, f32 cx, f32 cy, f32 scrollOffs)
{
  if(s->imgTex == 0 || !isInBounds(cx, cy, s->imgDim)) {
    return;
  }
  s->scrollLevel += scrollOffs;
  f32 icx = cx - s->imgDim[0];
  f32 icy = cy - s->imgDim[1];
  f32 oldtw = s->texScrollDim[2] - s->texScrollDim[0];
  f32 oldth = s->texScrollDim[3] - s->texScrollDim[1];
  f32 tcx = icx * (oldtw / (s->imgDim[2] - s->imgDim[0])) + s->texScrollDim[0];
  f32 tcy = icy * (oldth / (s->imgDim[3] - s->imgDim[1])) + s->texScrollDim[1];
  s->zoomCenter.vec[0] = tcx;
  s->zoomCenter.vec[1] = tcy;
  f32 scaling = 1.0 + 0.05 * s->scrollLevel;
  f32 newW = s->imgTex->imgWidth * scaling;
  f32 newH = s->imgTex->imgHeight * scaling;
  f32 tx = tcx * (1.0 - scaling);
  f32 ty = tcy * (1.0 - scaling);
  printf("%f %f\n", tx, ty);
  s->texScrollDim[0] = tx;
  s->texScrollDim[1] = ty;
  s->texScrollDim[2] = newW + tx;
  s->texScrollDim[3] = newH + ty;
  updateTexCoordData(&s->imgTexCoord, tx, ty,
                                     s->texScrollDim[2], s->texScrollDim[3]);
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

