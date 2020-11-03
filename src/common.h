#ifndef SEBSPHOTOS_COMMON
#define SEBSPHOTOS_COMMON

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

#include <stdint.h>


typedef int32_t bool;

typedef int32_t i32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef float f32;
typedef double f64;

struct ShaderProgram;

extern u16 IBO_DATA_SQUARE[6];

// ORDER MATTERS!
enum ModifiedState {
  NOT_MOD = 0,
  MOD_REPLACE = 1,
  MOD_REALLOC = 2
};

struct VBO { // always f32 data
  GLuint id;
  GLuint loc;
  enum ModifiedState mod;
  u32 count;
  u32 dim;
  u32 byteSize;
  f32 *data;
};
typedef struct VBO VBO;

struct IBO { // always u16 data
  GLuint id;
  enum ModifiedState mod;
  u32 count;
  u32 byteSize;
  u16 *data;
};
typedef struct IBO IBO;

struct Uniformf {
  GLuint loc;
  u32 dim;
  f32 vec[4];
};
typedef struct Uniformf Uniformf;

// -------------------------------------------------------------------------

void initIBO(IBO *ibo, u32 c);
void initVBO(VBO *vbo, u32 c, u32 d, struct ShaderProgram *p, char *an);

void updateVBOData(VBO *vbo, u32 count, u32 offs, f32 *data);
void updateIBOData(IBO *vbo, u32 count, u32 offs, u16 *data);

void updateVBO(VBO *vbo);
void updateIBO(IBO *ibo);

void setIBOMod(IBO *ibo, enum ModifiedState m);
void setVBOMod(VBO *vbo, enum ModifiedState m);

// -------------------------------------------------------------------------

void initUniformf(Uniformf *u, u32 dim, struct ShaderProgram* p, char *un);
void loadUniformf(Uniformf *u);

// -------------------------------------------------------------------------

void setGeomData(VBO* vbo, bool norm, u32 offs, f32 x0, f32 y0, f32 x1, f32 y1);
void setTexCData(VBO* vbo, u32 offs, f32 u0, f32 v0, f32 u1, f32 v1);

bool isInBounds(f32 x, f32 y, f32 *dims);
void toClipSpace(f32 *array, u32 count, u32 offs);
void setZeros(f32 *array, u32 len);

inline f32 minf(f32 a, f32 b) {
  return a < b ? a : b;
}
inline f32 maxf(f32 a, f32 b) {
  return a > b ? a : b;
}

#endif
