#include "scenewim.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *scene = malloc(sizeof(SceneWim));
  initImgShader(scene, shaderDir);
  initImgIndices(scene);
  initImgGeom(scene);
  initImgTexCoord(scene);
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
  s->imgIndices.type = GL_SHORT;
  s->imgIndices.count = 6;
  s->imgIndices.byteSize = sizeof(u16) * s->imgIndices.count * 1;
  s->imgIndices.isModified = 0;
  s->imgIndices.data = malloc(s->imgIndices.byteSize);
  *arrayu16_at(s->imgIndices.data, 0) = 0;
  *arrayu16_at(s->imgIndices.data, 1) = 1;
  *arrayu16_at(s->imgIndices.data, 2) = 2;
  *arrayu16_at(s->imgIndices.data, 3) = 2;
  *arrayu16_at(s->imgIndices.data, 4) = 3;
  *arrayu16_at(s->imgIndices.data, 5) = 0;
}

void initImgGeom(SceneWim *s)
{
  s->imgGeom.id = 0;
  s->imgGeom.type = GL_FLOAT;
  s->imgGeom.count = 4;
  s->imgGeom.byteSize = sizeof(f32) * s->imgGeom.count * 4;
  s->imgGeom.isModified = 0;
  s->imgGeom.data = malloc(s->imgGeom.byteSize);
  for(u32 i = 0; i < 16; i++) {
    if(i % 4 == 2) { *arrayf32_at(s->imgGeom.data, i) = 0.0; } // z coord
    else if(i % 4 == 3) { *arrayf32_at(s->imgGeom.data, i) = 1.0; } // w coord
    else { *arrayf32_at(s->imgGeom.data, i) = 0.0; }
  }
}

void initImgTexCoord(SceneWim *s)
{
  s->imgTexCoord.id = 0;
  s->imgTexCoord.type = GL_FLOAT;
  s->imgTexCoord.count = 4;
  s->imgTexCoord.byteSize = sizeof(f32) * s->imgTexCoord.count * 2;
  s->imgTexCoord.isModified = 0;
  s->imgTexCoord.data = malloc(s->imgTexCoord.byteSize);
  for(u32 i = 0; i < 8; i++) {
    *arrayf32_at(s->imgTexCoord.data, i) = 0.0;
  }
}

// Should be called after geometry is initialized
void createBuffers(SceneWim *s)
{
  glGenBuffers(1, &s->imgGeom.id);
  glGenBuffers(1, &s->imgTexCoord.id);
  // Vertex Buffers
  glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
  glBufferData(GL_ARRAY_BUFFER, s->imgGeom.byteSize, s->imgGeom.data,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgTexCoord.id);
  glBufferData(GL_ARRAY_BUFFER, s->imgTexCoord.byteSize, s->imgTexCoord.data,
               GL_STATIC_DRAW);
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
  }
  if(s->imgTexCoord.isModified) {
    glBindBuffer(GL_ARRAY_BUFFER, s->imgTexCoord.id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, s->imgTexCoord.byteSize,
                    s->imgTexCoord.data);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void updateImgGeom(SceneWim *s, f32 x0, f32 y0, f32 x1, f32 y1)
{
  *arrayf32_at(s->imgGeom.data, 0)  = x0;
  *arrayf32_at(s->imgGeom.data, 1)  = y1; // upper left
  *arrayf32_at(s->imgGeom.data, 4)  = x1;
  *arrayf32_at(s->imgGeom.data, 5)  = y1; // upper right
  *arrayf32_at(s->imgGeom.data, 8)  = x1;
  *arrayf32_at(s->imgGeom.data, 9)  = y0; // lower right
  *arrayf32_at(s->imgGeom.data, 12) = x0;
  *arrayf32_at(s->imgGeom.data, 13) = y0; // lower left
  s->imgGeom.isModified = 1;
}

void updateImgTexCoord(SceneWim *s, f32 u0, f32 v0, f32 u1, f32 v1)
{
  *arrayf32_at(s->imgTexCoord.data, 0) = u0;
  *arrayf32_at(s->imgTexCoord.data, 1) = v0; // lower left
  *arrayf32_at(s->imgTexCoord.data, 2) = u1;
  *arrayf32_at(s->imgTexCoord.data, 3) = v0; // lower right
  *arrayf32_at(s->imgTexCoord.data, 4) = u1;
  *arrayf32_at(s->imgTexCoord.data, 5) = v1; // upper right
  *arrayf32_at(s->imgTexCoord.data, 6) = u0;
  *arrayf32_at(s->imgTexCoord.data, 7) = v1; // upper left
  s->imgTexCoord.isModified = 1;
}

void drawSceneWim(SceneWim *s)
{
  updateBuffers(s);
  glUseProgram(s->imgShader.prog);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgGeom.id);
  // TODO: Get valid attribute locations!
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, s->imgTexCoord.id);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->imgIndices.id);
  glDrawElements(GL_TRIANGLES, s->imgIndices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glUseProgram(0);
  // FPS print
  //f64 now = getCurrentTime();
  //printf("%.2f ms\n", (now - scene->lastDrawTime) * 1000.0);
  //scene->lastDrawTime = now;
}


//createGeometry(&p->vertexBuffer, &p->textureBuffer, &p->indexBuffer);
//TextureContainer *texCont = allocTextureFromJPG(texFilePath);
//createTexture(&p->texObject, texCont);
//freeTexture(texCont);

/*
void createGeometry(GLuint *vb, GLuint *tb, GLuint *ib)
{
}

void createTexture(GLuint *texObj, TextureContainer *texCont)
{
  glGenTextures(1, texObj);
  glBindTexture(GL_TEXTURE_2D, *texObj);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               texCont->texWidth, texCont->texHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, texCont->texBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
}
*/

// ---------------------------------------------------------------------------

f32 *arrayf32_at(void *data, u32 index)
{
  return &((f32*) data)[index];
}

i32 *arrayi32_at(void *data, u32 index)
{
  return &((i32*) data)[index];
}

u16 *arrayu16_at(void *data, u32 index)
{
  return &((u16*) data)[index];
}

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

