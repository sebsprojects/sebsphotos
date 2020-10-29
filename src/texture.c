#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <math.h>

#include "texture.h"


Texture *allocTextureFromJPG(char *filePath)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE *file = fopen(filePath, "rb");
  if(!file) {
    printf("Error opening jpg file");
    return 0;
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, 1);
  jpeg_start_decompress(&cinfo);

  Texture *t = allocEmptyTexture(cinfo.output_width, cinfo.output_height);
  i32 pixelSize = cinfo.output_components;
  i32 rowStride = t->texWidth * pixelSize;
  t->imgBuffer = calloc(t->texWidth * t->texHeight * pixelSize, sizeof(u8));

  while(cinfo.output_scanline < cinfo.output_height) {
    u8 *rowBufferArray[1];
    rowBufferArray[0] = t->imgBuffer + cinfo.output_scanline * rowStride;
    jpeg_read_scanlines(&cinfo, rowBufferArray, 1);
  }

  jpeg_finish_decompress(&cinfo);
  fclose(file);
  jpeg_destroy_decompress(&cinfo);
  return t;
}

Texture *allocEmptyTexture(u32 width, u32 height)
{
  Texture *t = malloc(sizeof(Texture));
  t->imgWidth = width;
  t->imgHeight = height;
  t->texWidth = pow(2, ceil(log(t->imgWidth)/log(2)));
  t->texHeight = pow(2, ceil(log(t->imgHeight)/log(2)));
  t->id = 0;
  t->samplerLoc = 0;
  t->imgBuffer = 0;
  return t;
}

void initTexture(Texture *t)
{
  glGenTextures(1, &t->id);
  glBindTexture(GL_TEXTURE_2D, t->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               t->texWidth, t->texHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, t->imgBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void destroyTexture(Texture *t)
{
  if(t->imgBuffer != 0) { free(t->imgBuffer); }
  free(t);
}

