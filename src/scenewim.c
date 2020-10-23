#include "scenewim.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


SceneWim *createSceneWim(char *shaderDir)
{
  SceneWim *scene = malloc(sizeof(SceneWim));
  ShaderProgram mainShader = {
    0, 0, 0, 0, 0, SHADER_NOERROR
  };
  scene->mainShader = mainShader;
  char *vertPath = allocFullShaderPath(shaderDir, "mainImage.vert");
  char *fragPath = allocFullShaderPath(shaderDir, "mainImage.frag");
  scene->mainShader.vertSrc = allocFileContent(vertPath);
  scene->mainShader.fragSrc = allocFileContent(fragPath);
  free(fragPath);
  free(vertPath);
  compileAndLinkShaderProgram(&scene->mainShader);
  if(scene->mainShader.error != SHADER_NOERROR) {
    printf("Error with code %i\n", scene->mainShader.error);
    //TODO: ERROR
  }
  return scene;
}

void destroySceneWim(SceneWim *scene)
{
  if(scene->mainShader.vertSrc != 0) {
    free(scene->mainShader.vertSrc);
  }
  if(scene->mainShader.fragSrc != 0) {
    free(scene->mainShader.fragSrc);
  }
  free(scene);
}

void drawSceneWim(SceneWim *scene)
{
  glUseProgram(scene->mainShader.prog);
  //TODO Index Number missing in arg #2
  //p->indexNum = sizeof(iData) / sizeof(u16);
  glDrawElements(GL_TRIANGLES, 0, GL_UNSIGNED_SHORT, 0);
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
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glGenBuffers(1, vb);
  glGenBuffers(1, tb);
  glGenBuffers(1, ib);
  glBindBuffer(GL_ARRAY_BUFFER, *vb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vData), vData, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, *tb);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tData), tData, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ib);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iData), iData, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static u8 tex[] = {
  255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255,
  255, 255
};

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
static f32 vData[] = {
  -0.9,  0.9, 0.0, 1.0,
   0.9,  0.9, 0.0, 1.0,
   0.9, -0.9, 0.0, 1.0,
  -0.9, -0.9, 0.0, 1.0
};
static f32 tData[] = {
  0.0, 0.0,
  1.0, 0.0,
  1.0, 1.0,
  0.0, 1.0
};
static u16 iData[] = {
  0, 1, 2, 2, 3, 0
};
*/

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

