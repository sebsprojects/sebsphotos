#include "text.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <font_droidsans_64d8.h>

#include "texture.h"


static Font *font = &font_Droid_Sans;
static Character *charInfo = characters_Droid_Sans;

static Texture *tex;

Text *createText(char *shaderDir)
{
  Text *t = malloc(sizeof(Text));
  t->text = 0;
  t->textLen = 0;
  initShader(&t->textShader, shaderDir, "text");
  initVBO(&t->geom, 0, 2, &t->textShader, "in_position");
  initVBO(&t->texC, 0, 2, &t->textShader, "in_texcoord");
  initIBO(&t->indices, 0);
  initUniformf(&t->res, 2, &t->textShader, "res");
  tex = allocTextureFromJPG("res/font_droidsans_64d8.jpg");
  initTexture(tex);
  return t;
}

void destroyText(Text *t)
{
  free(t);
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
  u32 totalAdvance = 20.0;
  for(u32 i = 0; i < newLen; i++) {
    i32 index = newText[i] - 32;
    //TODO: Bounds check?
    Character c = charInfo[index];
    f32 x0 = totalAdvance - c.originX;
    f32 y0 = 20.0 + c.originY - c.height;
    f32 x1 = totalAdvance - c.originX + c.width;
    f32 y1 = 20.0 + c.originY;
    f32 u0 = (f32) c.x / font->width;
    f32 v0 = (f32) c.y / font->height;
    f32 u1 = (f32) (c.x + c.width) / font->width;
    f32 v1 = (f32) (c.y + c.height) / font->height;
    setGeomData(&t->geom, 0, i * 8, x0, y0, x1, y1);
    setTexCData(&t->texC, i * 8, u0, v0, u1, v1);
    setStandardIndexData(&t->indices, i * 6, i * 4);
    totalAdvance += c.advance;
    //printf("i=%u index=%i char=%c\n", i, index, charInfo[index].codePoint);
    printf("%c :: %f %f %f %f\n", c.codePoint, x0, y0, x1, y1);
  }
  updateVBO(&t->geom);
  updateVBO(&t->texC);
  updateIBO(&t->indices);
}

void updatePos(Text *t, f32 x, f32 y)
{

}

void drawText(Text *t)
{
  glUseProgram(t->textShader.prog);
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

  loadUniformf(&t->res);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->indices.id);
  glDrawElements(GL_TRIANGLES, t->indices.count, GL_UNSIGNED_SHORT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(t->geom.loc);
  glUseProgram(0);
}
