#include "common.h"

#include <stdlib.h>
#include <stdio.h>

#include "shader.h"

u16 IBO_DATA_SQUARE[6] = { 0, 1, 2, 2, 3, 0 };

void initUniformf(Uniformf *u, u32 dim, ShaderProgram* p, char *un)
{
  u->dim = dim;
  setZeros(u->vec, 4);
  if(p != 0) {
    u->loc = glGetUniformLocation(p->prog, un);
    printf("p=%p name=%s loc=%i\n", p, un, u->loc);
  }
}

void loadUniformf(Uniformf *u)
{
  if(u->dim == 1) { glUniform1f(u->loc, u->vec[0]); }
  if(u->dim == 2) { glUniform2f(u->loc, u->vec[0], u->vec[1]); }
  if(u->dim == 3) { glUniform3f(u->loc, u->vec[0], u->vec[1], u->vec[2]); }
  if(u->dim == 4) {
    glUniform4f(u->loc, u->vec[0], u->vec[1], u->vec[2], u->vec[3]);
  }
}

void initIBO(IBO *ibo, u32 c)
{
  ibo->id = 0;
  ibo->count = c;
  ibo->byteSize = sizeof(u16) * ibo->count;
  ibo->mod = MOD_REALLOC;
  ibo->data = calloc(c, sizeof(u16));
  glGenBuffers(1, &ibo->id);
}

void initVBO(VBO *vbo, u32 c, u32 d, ShaderProgram *p, char *an)
{
  vbo->id = 0;
  vbo->loc = 0;
  vbo->count = c;
  vbo->dim = d;
  vbo->byteSize = sizeof(f32) * vbo->count * vbo->dim;
  vbo->mod = MOD_REALLOC;
  vbo->data = calloc(c * d, sizeof(f32));
  glGenBuffers(1, &vbo->id);
  if(p != 0) {
    vbo->loc = glGetAttribLocation(p->prog, an);
  }
}

void updateVBOData(VBO *vbo, u32 count, u32 offs, f32 *data)
{
  if(vbo->count < offs + count) {
    vbo->byteSize = (offs + count) * vbo->dim * sizeof(f32);
    setVBOMod(vbo, MOD_REALLOC);
    vbo->data = (f32*) realloc(vbo->data, vbo->byteSize);
  } else {
    setVBOMod(vbo, MOD_REPLACE);
  }
  for(u32 i = 0; i < count; i++) {
    vbo->data[offs + i] = data[i];
  }
}

void updateIBOData(IBO *ibo, u32 count, u32 offs, u16 *data) {
  if(ibo->count < offs + count) {
    ibo->byteSize = (offs + count) * sizeof(u16);
    setIBOMod(ibo, MOD_REALLOC);
    ibo->data = (u16*) realloc(ibo->data, ibo->byteSize);
  } else {
    setIBOMod(ibo, MOD_REPLACE);
  }
  for(u32 i = 0; i < count; i++) {
    ibo->data[offs + i] = data[i];
  }
}

void updateVBO(VBO *vbo)
{
  if(vbo->mod == NOT_MOD) {
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
  if(vbo->mod == MOD_REPLACE) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbo->byteSize, vbo->data);
  }
  if (vbo->mod == MOD_REALLOC) {
    glBufferData(GL_ARRAY_BUFFER, vbo->byteSize, vbo->data, GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  vbo->mod = NOT_MOD;
}

void updateIBO(IBO *ibo)
{
  if(ibo->mod == NOT_MOD) {
    return;
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->id);
  if(ibo->mod == MOD_REPLACE) {
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ibo->byteSize, ibo->data);
  }
  if(ibo->mod == MOD_REALLOC) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibo->byteSize, ibo->data,
                 GL_STATIC_DRAW);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  ibo->mod = NOT_MOD;
}

void setIBOMod(IBO *ibo, enum ModifiedState m)
{
  if(ibo->mod < m) {
    ibo->mod = m;
  }
}

void setVBOMod(VBO *vbo, enum ModifiedState m)
{
  if(vbo->mod < m) {
    vbo->mod = m;
  }
}

// -------------------------------------------------------------------------

void setGeomData(VBO *vbo, bool norm, u32 offs, f32 x0, f32 y0, f32 x1, f32 y1)
{
  vbo->data[offs + 0] = x0; vbo->data[offs + 1] = y1; // upper left
  vbo->data[offs + 2] = x1; vbo->data[offs + 3] = y1; // upper right
  vbo->data[offs + 4] = x1; vbo->data[offs + 5] = y0; // lower right
  vbo->data[offs + 6] = x0; vbo->data[offs + 7] = y0; // lower left
  if(norm) {
    toClipSpace(vbo->data, 8, offs);
  }
  setVBOMod(vbo, MOD_REPLACE);
}

void setTexCData(VBO *vbo, u32 offs, f32 u0, f32 v0, f32 u1, f32 v1)
{
  vbo->data[offs + 0] = u0; vbo->data[offs + 1] = v0; // lower left
  vbo->data[offs + 2] = u1; vbo->data[offs + 3] = v0; // lower right
  vbo->data[offs + 4] = u1; vbo->data[offs + 5] = v1; // upper right
  vbo->data[offs + 6] = u0; vbo->data[offs + 7] = v1; // upper left
  setVBOMod(vbo, MOD_REPLACE);
}

bool isInBounds(f32 x, f32 y, f32 *dims)
{
  return x >= dims[0] && y >= dims[1] && x <= dims[2] && y <= dims[3];
}

void toClipSpace(f32 *array, u32 count, u32 offs)
{
  for(u32 i = offs; i < offs + count; i++) {
    array[i] = array[i] * 2.0 - 1.0;
  }
}

void setZeros(f32 *array, u32 len)
{
  for(u32 i = 0; i < len; i++) {
    array[i] = 0.0;
  }
}

