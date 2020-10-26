#include "exporter.h"

#include <stdlib.h>


Exporter *createExporter()
{
  Exporter *e = malloc(sizeof(Exporter));
  /*
  e->fboTex = allocEmptyTexture(4096, 4096);
  initTexture(e->fboTex);
  glGenFramebuffers(1, &e->fboId);
  glBindFramebuffer(e->fboId);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, e->fboTex->id, 0);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Error with framebuffer completeness\n");
  }
  glBindFramebuffer(0);
  */
}

void destroyExporter(Exporter *e)
{
  if(e->exportShader.vertSrc != 0) { free(e->exportShader.vertSrc); }
  if(e->exportShader.fragSrc != 0) { free(e->exportShader.fragSrc); }
  if(e->geom.data != 0) { free(e->geom.data); }
  if(e->texCoord.data != 0) { free(e->texCoord.data); }
  if(e->indices.data != 0) { free(e->indices.data); }
  if(e->fboTex != 0) { destroyTexture(e->fboTex); }
  free(e);
}

