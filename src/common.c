#include "common.h"

#include <stdlib.h>
#include <stdio.h>

#include "shader.h"


void loadUniformf(Uniformf *u)
{
  if(u->dim == 1) { glUniform1f(u->loc, u->vec[0]); }
  if(u->dim == 2) { glUniform2f(u->loc, u->vec[0], u->vec[1]); }
  if(u->dim == 3) { glUniform3f(u->loc, u->vec[0], u->vec[1], u->vec[2]); }
  if(u->dim == 4) {
    glUniform4f(u->loc, u->vec[0], u->vec[1], u->vec[2], u->vec[3]);
  }
}

void genVBO(VBO *vbo)
{
  glGenBuffers(1, &vbo->id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
  glBufferData(GL_ARRAY_BUFFER, vbo->byteSize, vbo->data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void genIBO(IBO *ibo)
{
  glGenBuffers(1, &ibo->id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo->byteSize, ibo->data,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void initStandardIBO(IBO *ibo)
{
  ibo->id = 0;
  ibo->count = 6;
  ibo->byteSize = sizeof(u16) * ibo->count;
  ibo->data = malloc(ibo->byteSize);
  ibo->data[0] = 0; ibo->data[1] = 1;
  ibo->data[2] = 2; ibo->data[3] = 2;
  ibo->data[4] = 3; ibo->data[5] = 0;
  genIBO(ibo);
}

void initStandardGeom(VBO *geom, ShaderProgram *p, char *attribName)
{
  geom->id = 0;
  geom->loc = 0;
  geom->count = 4;
  geom->dim = 4;
  geom->byteSize = sizeof(f32) * geom->count * geom->dim;
  geom->isModified = 0;
  geom->data = malloc(geom->byteSize);
  setZeros(geom->data, 16);
  for(u32 i = 0; i < 4; i++) {
    geom->data[3 + i * 4] = 1.0;
  }
  if(p != 0) {
    geom->loc = glGetAttribLocation(p->prog, attribName);
  }
  genVBO(geom);
}

void initStandardTexCoord(VBO *texCoord, ShaderProgram *p, char *attribName)
{
  texCoord->id = 0;
  texCoord->loc = 0;
  texCoord->count = 4;
  texCoord->dim = 2;
  texCoord->byteSize =
    sizeof(f32) * texCoord->count * texCoord->dim;
  texCoord->isModified = 0;
  texCoord->data = malloc(texCoord->byteSize);
  setZeros(texCoord->data, 8);
  if(p != 0) {
    texCoord->loc = glGetAttribLocation(p->prog, attribName);
  }
  genVBO(texCoord);
}

void initStandardUniformf(Uniformf *u, u32 dim, ShaderProgram* p, char *un)
{
  u->dim = dim;
  setZeros(u->vec, 4);
  if(p != 0) {
    u->loc = glGetUniformLocation(p->prog, un);
  }
}

void updateGeomData(VBO *vbo, f32 x0, f32 y0, f32 x1, f32 y1)
{
  vbo->data[0]  = x0; vbo->data[1]  = y1; // upper left
  vbo->data[4]  = x1; vbo->data[5]  = y1; // upper right
  vbo->data[8]  = x1; vbo->data[9]  = y0; // lower right
  vbo->data[12] = x0; vbo->data[13] = y0; // lower left
  vbo->isModified = 1;
}

void updateTexCoordData(VBO *vbo, f32 u0, f32 v0, f32 u1, f32 v1)
{
  vbo->data[0] = u0; vbo->data[1] = v0; // lower left
  vbo->data[2] = u1; vbo->data[3] = v0; // lower right
  vbo->data[4] = u1; vbo->data[5] = v1; // upper right
  vbo->data[6] = u0; vbo->data[7] = v1; // upper left
  vbo->isModified = 1;
}

void updateVBO(VBO *vbo)
{
  if(vbo->isModified) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbo->byteSize, vbo->data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vbo->isModified = 0;
  }
}

// -------------------------------------------------------------------------

bool isInBounds(f32 x, f32 y, f32 *dims)
{
  return x >= dims[0] && y >= dims[1] && x <= dims[2] && y <= dims[3];
}

void setZeros(f32 *array, u32 len)
{
  for(u32 i = 0; i < len; i++) {
    array[i] = 0.0;
  }
}

