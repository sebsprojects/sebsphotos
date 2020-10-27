#ifndef SEBSPHOTOS_EXPORTER
#define SEBSPHOTOS_EXPORTER

#include "common.h"
#include "texture.h"
#include "shader.h"


struct ExportInfo {
  f32 texCoord[4];
  i32 viewportCoord[4];
  Texture *tex;
};
typedef struct ExportInfo ExportInfo;

struct Exporter {
  ShaderProgram exportShader;
  VBO geom;
  VBO texCoord;
  IBO indices;
  Uniformf texDim;
  GLuint fboId;
  Texture *fboTex;
};
typedef struct Exporter Exporter;

Exporter *createExporter(char *shaderDir);
void destroyExporter(Exporter *e);


void exportImage(char *exportPath, ExportInfo inf);

#endif
