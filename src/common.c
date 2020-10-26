#include "common.h"


void loadUniformf(Uniformf *u)
{
  if(u->dim == 1) { glUniform1f(u->loc, u->vec[0]); }
  if(u->dim == 2) { glUniform2f(u->loc, u->vec[0], u->vec[1]); }
  if(u->dim == 3) { glUniform3f(u->loc, u->vec[0], u->vec[1], u->vec[2]); }
  if(u->dim == 4) {
    glUniform4f(u->loc, u->vec[0], u->vec[1], u->vec[2], u->vec[3]);
  }
}

bool isInBounds(f32 x, f32 y, f32 *dims)
{
  return x >= dims[0] && y >= dims[1] && x <= dims[2] && y <= dims[3];
}
