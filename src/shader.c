#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shader.h"


void initStandardShader(ShaderProgram *p, char* shaderDir, char *shaderName)
{
  p->vert = 0; p->frag = 0; p-> prog = 0;
  p->error = SHADER_NOERROR;
  char *vertPath = allocFullShaderPath(shaderDir, shaderName, "vert");
  char *fragPath = allocFullShaderPath(shaderDir, shaderName, "frag");
  p->vertSrc = allocFileContent(vertPath);
  p->fragSrc = allocFileContent(fragPath);
  free(fragPath);
  free(vertPath);
  if(p->error != SHADER_NOERROR) {
    printf("Error with code %i\n", p->error);
    //TODO: ERROR
  }
  compileAndLinkShader(p);
}

void compileAndLinkShader(ShaderProgram *p)
{
  const GLchar *vertSrc = p->vertSrc;
  const GLchar *fragSrc = p->fragSrc;
  GLint success = 0;
  p->vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(p->vert, 1, &vertSrc, 0);
  glCompileShader(p->vert);
  glGetShaderiv(p->vert, GL_COMPILE_STATUS, &success);
  if(success == GL_FALSE) {
    GLint len = 0;
    glGetShaderiv(p->vert, GL_INFO_LOG_LENGTH, &len);
    GLchar *log = malloc(len * sizeof(GLchar));
    glGetShaderInfoLog(p->vert, len, &len, log);
    fprintf(stderr, "Error compiling vertex shader: %s\n", log);
    free(log);
    glDeleteShader(p->vert);
    p->error = SHADER_ERROR_COMPILE_VERT;
    return;
  }
  p->frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(p->frag, 1, &fragSrc, 0);
  glCompileShader(p->frag);
  glGetShaderiv(p->frag, GL_COMPILE_STATUS, &success);
  if(success == GL_FALSE) {
    GLint len = 0;
    glGetShaderiv(p->frag, GL_INFO_LOG_LENGTH, &len);
    GLchar *log = malloc(len * sizeof(GLchar));
    glGetShaderInfoLog(p->frag, len, &len, log);
    fprintf(stderr, "Error compiling fragment shader: %s\n", log);
    free(log);
    glDeleteShader(p->vert);
    glDeleteShader(p->frag);
    p->error = SHADER_ERROR_COMPILE_FRAG;
    return;
  }
  p->prog = glCreateProgram();
  glAttachShader(p->prog, p->vert);
  glAttachShader(p->prog, p->frag);
  glLinkProgram(p->prog);
  glGetProgramiv(p->prog, GL_LINK_STATUS, &success);
  if(success == GL_FALSE) {
    GLint len = 0;
    glGetProgramiv(p->prog, GL_INFO_LOG_LENGTH, &len);
    GLchar *log = malloc(len * sizeof(GLchar));
    glGetProgramInfoLog(p->prog, len, &len, log);
    fprintf(stderr, "Error linking shader: %s\n", log);
    free(log);
    glDeleteShader(p->vert);
    glDeleteShader(p->frag);
    glDeleteProgram(p->prog);
    p->error = SHADER_ERROR_LINK;
    return;
  }
  glDetachShader(p->prog, p->vert);
  glDetachShader(p->prog, p->frag);
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

char *allocFullShaderPath(char *baseDir, char *fileName, char *ext)
{
  // add baseDir + / + fileName + . + ext + \0
  char *fullPath =
    malloc(strlen(baseDir) + strlen(fileName) + strlen(ext) + 1 + 1 + 1);
  strcpy(fullPath, baseDir);
  strcat(fullPath, "/");
  strcat(fullPath, fileName);
  strcat(fullPath, ".");
  strcat(fullPath, ext);
  printf("full path %s\n", fullPath);
  return fullPath;
}

