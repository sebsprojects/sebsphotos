#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <math.h>

#include "jpg.h"


TextureContainer *allocTextureFromJPG(char *filePath)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE *file = fopen(filePath, "rb");
  if(!file) {
    printf("Error opening jpg file");
    return 0;
  }
  TextureContainer *tc = malloc(sizeof(TextureContainer));

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, 1);
  jpeg_start_decompress(&cinfo);

  tc->imgWidth = cinfo.output_width;
  tc->imgHeight = cinfo.output_height;
  tc->texWidth = pow(2, ceil(log(tc->imgWidth)/log(2)));
  tc->texHeight = pow(2, ceil(log(tc->imgHeight)/log(2)));

  i32 pixelSize = cinfo.output_components;
  i32 rowStride = tc->texWidth * pixelSize;
  tc->texBuffer = calloc(tc->texWidth * tc->texHeight * pixelSize, sizeof(u8));

  while(cinfo.output_scanline < cinfo.output_height) {
    u8 *rowBufferArray[1];
    rowBufferArray[0] = tc->texBuffer + cinfo.output_scanline * rowStride;
    jpeg_read_scanlines(&cinfo, rowBufferArray, 1);
  }

  jpeg_finish_decompress(&cinfo);
  fclose(file);
  jpeg_destroy_decompress(&cinfo);
  return tc;
}

void freeTexture(TextureContainer *tc) {
  free(tc->texBuffer);
  free(tc);
}
