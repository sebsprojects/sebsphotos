#include "text.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <font_droidsans_64d8.h>
#include <font_inconsolata_64d8.h>

#include "texture.h"


//static Font *font = &font_Droid_Sans;
static Font *font = &font_Inconsolata;


static Texture *tex = 0;
static f32 fontSize = 64.0;

void initText(Text *t, ShaderProgram *rel_textShader, Uniformf *rel_res)
{
  if(tex == 0) {
    //tex = allocTextureFromJPG("res/font_droidsans_64d8.jpg");
    tex = allocTextureFromJPG("res/font_inconsolata_64d8.jpg");
  }
  t->text = 0;
  t->textLen = 0;
  t->rel_res = rel_res;
  t->rel_textShader = rel_textShader;
  initVBO(&t->geom, 0, 2, t->rel_textShader, "in_position");
  initVBO(&t->texC, 0, 2, t->rel_textShader, "in_texcoord");
  initIBO(&t->indices, 0);
  initUniformf(&t->uDims, 4, t->rel_textShader, "textDims");
  t->uDims.vec[3] = fontSize;
  initTexture(tex, GL_LINEAR, GL_LINEAR);
}

void destroyText(Text *t)
{
  if(t->text != 0) { free(t->text); }
  if(t->geom.data != 0) { free(t->geom.data); }
  if(t->texC.data != 0) { free(t->texC.data); }
  if(t->indices.data != 0) { free(t->indices.data); }
}

void updateText(Text *t, char *newText)
{
  u32 newLen = strlen(newText);
  if(newLen > t->textLen) {
    t->text = (char*) realloc(t->text, newLen + 1);
    t->textLen = newLen;
    strcpy(t->text, newText);
  }
  // Only resize buffers
  if(t->geom.count < newLen * 4) {
    updateVBOData(&t->geom, newLen * 4, 0, 0);
    updateVBOData(&t->texC, newLen * 4, 0, 0);
    updateIBOData(&t->indices, newLen * 6, 0, 0);
  }
  // Update buffers inplace
  f32 totalAdvance = 0.0;
  for(u32 i = 0; i < newLen; i++) {
    i32 index = newText[i] - 32;
    //TODO: Bounds check?
    Character c = font->characters[index];
    if(i == 0) {
      totalAdvance = c.originX;
    }
    f32 scale = 1.0;
    f32 x0 = totalAdvance - c.originX;
    f32 y0 = c.originY - c.height;
    f32 x1 = totalAdvance - c.originX + c.width;
    f32 y1 = c.originY;
    f32 u0 = (f32) c.x / font->width;
    f32 v0 = (f32) c.y / font->height;
    f32 u1 = (f32) (c.x + c.width) / font->width;
    f32 v1 = (f32) (c.y + c.height) / font->height;
    setGeomData(&t->geom, 0, i * 8, scale * x0, scale * y0, scale * x1, scale * y1);
    setTexCData(&t->texC, i * 8, u0, v0, u1, v1);
    setStandardIndexData(&t->indices, i * 6, i * 4);
    totalAdvance += c.advance;
    //printf("i=%u index=%i char=%c\n", i, index, charInfo[index].codePoint);
    //printf("%c :: %f %f %f %f\n", c.codePoint, x0, y0, x1, y1);
  }
  updateVBO(&t->geom);
  updateVBO(&t->texC);
  updateIBO(&t->indices);
}

void updateTextDimensions(Text *t, f32 x, f32 y, f32 size)
{
  t->uDims.vec[0] = x;
  t->uDims.vec[1] = y;
  t->uDims.vec[2] = size;
}

void drawText(Text *t)
{
  glUseProgram(t->rel_textShader->prog);
  glBindBuffer(GL_ARRAY_BUFFER, t->geom.id);
  glVertexAttribPointer(t->geom.loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(t->geom.loc);
  glBindBuffer(GL_ARRAY_BUFFER, t->texC.id);
  glVertexAttribPointer(t->texC.loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(t->texC.loc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex->id);
  glUniform1i(tex->samplerLoc, 0); // Value is texture unit num

  loadUniformf(t->rel_res);
  loadUniformf(&t->uDims);
  //printf("%f %f\n", t->uDims.vec[2], t->uDims[3]);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->indices.id);
  glDrawElements(GL_TRIANGLES, t->indices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(t->geom.loc);
  glUseProgram(0);
}
