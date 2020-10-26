#include "scenewim.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *scene = malloc(sizeof(SceneWim));
  scene->imgTex = 0;
  scene->res[0] = 0.0; scene->res[1] = 0.0;
  scene->sceneDim[0] = 0.0; scene->sceneDim[1] = 0.0;
  scene->sceneDim[2] = 0.0; scene->sceneDim[3] = 0.0;
  scene->scrollLevel = 0.0;
  initImgShader(scene, shaderDir);
  initImgIndices(scene);
  initImgGeom(scene);
  initImgTexCoord(scene);
  initUniforms(scene);
  createBuffers(scene);
  return scene;
}

void destroySceneWim(SceneWim *scene)
{
  if(scene->imgShader.vertSrc != 0) { free(scene->imgShader.vertSrc); }
  if(scene->imgShader.fragSrc != 0) { free(scene->imgShader.fragSrc); }
  if(scene->imgGeom.data != 0) { free(scene->imgGeom.data); }
  if(scene->imgTexCoord.data != 0) { free(scene->imgTexCoord.data); }
  if(scene->imgIndices.data != 0) { free(scene->imgIndices.data); }
  free(scene);
}

void initImgShader(SceneWim *s, char *shaderDir)
{
  ShaderProgram imgShader = { 0, 0, 0, 0, 0, SHADER_NOERROR };
  s->imgShader = imgShader;
  char *vertPath = allocFullShaderPath(shaderDir, "mainImage.vert");
  char *fragPath = allocFullShaderPath(shaderDir, "mainImage.frag");
  s->imgShader.vertSrc = allocFileContent(vertPath);
  s->imgShader.fragSrc = allocFileContent(fragPath);
  free(fragPath);
  free(vertPath);
  compileAndLinkShaderProgram(&s->imgShader);
  if(s->imgShader.error != SHADER_NOERROR) {
    printf("Error with code %i\n", s->imgShader.error);
    //TODO: ERROR
  }
}

void initImgIndices(SceneWim *s)
{
  s->imgIndices.id = 0;
  s->imgIndices.count = 6;
  s->imgIndices.byteSize = sizeof(u16) * s->imgIndices.count;
  s->imgIndices.data = malloc(s->imgIndices.byteSize);
  s->imgIndices.data[0] = 0; s->imgIndices.data[1] = 1;
  s->imgIndices.data[2] = 2; s->imgIndices.data[3] = 2;
  s->imgIndices.data[4] = 3; s->imgIndices.data[5] = 0;
}

void initImgGeom(SceneWim *s)
{
  s->imgGeom.id = 0;
  s->imgGeom.loc = 0;
  s->imgGeom.count = 4;
  s->imgGeom.dim = 4;
  s->imgGeom.byteSize = sizeof(f32) * s->imgGeom.count * s->imgGeom.dim;
  s->imgGeom.isModified = 0;
  s->imgGeom.data = malloc(s->imgGeom.byteSize);
  for(u32 i = 0; i < 16; i++) {
    if(i % 4 == 2) { s->imgGeom.data[i] = 0.0; } // z coord
    else if(i % 4 == 3) { s->imgGeom.data[i] = 1.0; } // w coord
    else { s->imgGeom.data[i] = 0.0; }
  }
  s->imgGeom.loc = glGetAttribLocation(s->imgShader.prog, "in_position");
}

void initImgTexCoord(SceneWim *s)
{
  s->imgTexCoord.id = 0;
  s->imgTexCoord.loc = 0;
  s->imgTexCoord.count = 4;
  s->imgTexCoord.dim = 2;
  s->imgTexCoord.byteSize =
    sizeof(f32) * s->imgTexCoord.count * s->imgTexCoord.dim;
  s->imgTexCoord.isModified = 0;
  s->imgTexCoord.data = malloc(s->imgTexCoord.byteSize);
  for(u32 i = 0; i < 8; i++) {
    s->imgTexCoord.data[i] = 0.0;
  }
  s->imgTexCoord.loc = glGetAttribLocation(s->imgShader.prog, "in_texCoord");
}

void initUniforms(SceneWim *s)
{
  s->imgTexDim.loc = glGetUniformLocation(s->imgShader.prog, "texDim");
  s->imgTexDim.dim = 2;
  s->imgTexDim.vec[0] = 0.0; s->imgTexDim.vec[1] = 0.0;
  s->imgTexDim.vec[2] = 0.0; s->imgTexDim.vec[3] = 0.0;
}

// Should be called after geometry is initialized
void createBuffers(SceneWim *s)
{
  glGenBuffers(1, &s->imgGeom.id);
  glGenBuffers(1, &s->imgTexCoord.id);
  // Vertex Buffers
  glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
  glBufferData(GL_ARRAY_BUFFER, s->imgGeom.byteSize,
               s->imgGeom.data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgTexCoord.id);
  glBufferData(GL_ARRAY_BUFFER, s->imgTexCoord.byteSize,
               s->imgTexCoord.data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Index Buffer
  glGenBuffers(1, &s->imgIndices.id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->imgIndices.id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, s->imgIndices.byteSize,
               s->imgIndices.data, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void updateBuffers(SceneWim *s)
{
  if(s->imgGeom.isModified) {
    glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, s->imgGeom.byteSize, s->imgGeom.data);
    s->imgGeom.isModified = 0;
  }
  if(s->imgTexCoord.isModified) {
    glBindBuffer(GL_ARRAY_BUFFER, s->imgTexCoord.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, s->imgTexCoord.byteSize,
                    s->imgTexCoord.data);
    s->imgTexCoord.isModified = 0;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateImgGeom(SceneWim *s, f32 x0, f32 y0, f32 x1, f32 y1)
{
  s->imgGeom.data[0]  = x0; s->imgGeom.data[1]  = y1; // upper left
  s->imgGeom.data[4]  = x1; s->imgGeom.data[5]  = y1; // upper right
  s->imgGeom.data[8]  = x1; s->imgGeom.data[9]  = y0; // lower right
  s->imgGeom.data[12] = x0; s->imgGeom.data[13] = y0; // lower left
  s->imgGeom.isModified = 1;
}

void updateImgTexCoord(SceneWim *s, f32 u0, f32 v0, f32 u1, f32 v1)
{
  s->imgTexCoord.data[0] = u0; s->imgTexCoord.data[1] = v0; // lower left
  s->imgTexCoord.data[2] = u1; s->imgTexCoord.data[3] = v0; // lower right
  s->imgTexCoord.data[4] = u1; s->imgTexCoord.data[5] = v1; // upper right
  s->imgTexCoord.data[6] = u0; s->imgTexCoord.data[7] = v1; // upper left
  s->imgTexCoord.isModified = 1;
}

void updateImgTex(SceneWim *s, Texture *tex)
{
  s->imgTex = tex;
  s->imgTex->samplerLoc = glGetUniformLocation(s->imgShader.prog, "tex");
  s->imgTexDim.vec[0] = tex->texWidth;
  s->imgTexDim.vec[1] = tex->texHeight;
  updateImgTexCoord(s, 0.0, 0.0, s->imgTex->imgWidth, s->imgTex->imgHeight);
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
    printf("imgDim: %f %f %f %f\n", s->imgDim[0], s->imgDim[1], s->imgDim[2],
           s->imgDim[3]);
    updateImgGeom(s, 2.0 * s->imgDim[0] / w - 1.0, 2.0 * s->imgDim[1] / h - 1.0,
                     2.0 * s->imgDim[2] / w - 1.0, 2.0 * s->imgDim[3] / h - 1.0);
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
  printf("old w/h: %f %f\n", oldtw, oldth);
  f32 tcx = icx * (oldtw / (s->imgDim[2] - s->imgDim[0])) + s->texScrollDim[0];
  f32 tcy = icy * (oldth / (s->imgDim[3] - s->imgDim[1])) + s->texScrollDim[1];
  printf("tcx tcy: %f %f\n", tcx, tcy);
  f32 scaling = 1.0 + 0.05 * s->scrollLevel;
  f32 tx = tcx * (1.0 - scaling);
  f32 ty = tcy * (1.0 - scaling);
  s->texScrollDim[0] = tx;
  s->texScrollDim[1] = ty;
  s->texScrollDim[2] = s->imgTex->imgWidth * scaling + tx;
  s->texScrollDim[3] = s->imgTex->imgHeight * scaling + ty;
  updateImgTexCoord(s, tx, ty,
                       s->texScrollDim[2], s->texScrollDim[3]);
}

void drawSceneWim(SceneWim *s)
{
  updateBuffers(s);
  glUseProgram(s->imgShader.prog);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
  // TODO: Get valid attribute locations!
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

// ---------------------------------------------------------------------------

char *allocFileContent(char *path)
{
  char *buf = 0;
  FILE *file = fopen(path, "rb");
  if(file) {
    fseek(file, 0, SEEK_END);
    i32 len = ftell(file);
    fseek(file, 0, SEEK_SET);
    buf = malloc(len + 1);
    u32 readLen = fread(buf, 1, len, file);
    buf[readLen] = '\0';
    fclose(file);
  } else {
    //TODO: ERROR!
    printf("error in allocFileContent\n");
    return 0;
  }
  return buf;
}

char *allocFullShaderPath(char *baseDir, char *fileName)
{
  // add baseDir + / + fileName + \0
  char *fullPath = malloc(strlen(baseDir) + strlen(fileName) + 1 + 1);
  strcpy(fullPath, baseDir);
  strcat(fullPath, "/");
  strcat(fullPath, fileName);
  printf("full path %s\n", fullPath);
  return fullPath;
}

