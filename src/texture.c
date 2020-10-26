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
  Texture *tc = malloc(sizeof(Texture));

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, 1);
  jpeg_start_decompress(&cinfo);

  tc->imgWidth = cinfo.output_width;
  tc->imgHeight = cinfo.output_height;
  tc->texWidth = pow(2, ceil(log(tc->imgWidth)/log(2)));
  tc->texHeight = pow(2, ceil(log(tc->imgHeight)/log(2)));
  tc->id = 0;
  tc->samplerLoc = 0;

  i32 pixelSize = cinfo.output_components;
  i32 rowStride = tc->texWidth * pixelSize;
  tc->imgBuffer = calloc(tc->texWidth * tc->texHeight * pixelSize, sizeof(u8));

  while(cinfo.output_scanline < cinfo.output_height) {
    u8 *rowBufferArray[1];
    rowBufferArray[0] = tc->imgBuffer + cinfo.output_scanline * rowStride;
    jpeg_read_scanlines(&cinfo, rowBufferArray, 1);
  }

  jpeg_finish_decompress(&cinfo);
  fclose(file);
  jpeg_destroy_decompress(&cinfo);
  return tc;
}

void initTexture(Texture *t)
{
  glGenTextures(1, &t->id);
  glBindTexture(GL_TEXTURE_2D, t->id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
               t->texWidth, t->texHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, t->imgBuffer);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void destroyTexture(Texture *t)
{
  free(t->imgBuffer);
  free(t);
}
