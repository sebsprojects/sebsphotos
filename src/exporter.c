#include "exporter.h"

#include <stdlib.h>
#include <stdio.h>


void initFBO(Exporter *e)
{
  glGenFramebuffers(1, &e->fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, e->fboId);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, e->fboTex->id, 0);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("Error with framebuffer completeness\n");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ---------------------------------------------------------------------------

Exporter *createExporter(char *shaderDir)
{
  Exporter *e = malloc(sizeof(Exporter));
  e->fboTex = allocEmptyTexture(4096, 4096);
  initTexture(e->fboTex, GL_NEAREST, GL_NEAREST);
  initFBO(e);
  initShader(&e->exportShader, shaderDir, "export");
  initIBO(&e->indices, 6);
  updateIBOData(&e->indices, 0, 6, IBO_DATA_SQUARE);
  initVBO(&e->geom, 16, 4, &e->exportShader, "in_position");
  initVBO(&e->texCoord, 8, 2, &e->exportShader, "in_texcoord");
  initUniformf(&e->texDim, 2, &e->exportShader, "texDim");
  setGeomData(&e->geom, 0, 0, -1.0, -1.0, 1.0, 1.0);
  return e;
}

void destroyExporter(Exporter *e)
{
  if(e->geom.data != 0) { free(e->geom.data); }
  if(e->texCoord.data != 0) { free(e->texCoord.data); }
  if(e->indices.data != 0) { free(e->indices.data); }
  if(e->fboTex != 0) { destroyTexture(e->fboTex); }
  free(e);
}

void exportImage(char *exportPath, ExportInfo inf)
{

}

