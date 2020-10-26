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

struct VBO { // always f32 data
  GLuint id;
  GLuint loc;
  bool isModified;
  u32 count;
  u32 dim;
  u32 byteSize;
  f32 *data;
};
typedef struct VBO VBO;

struct IBO { // always u16 data
  GLuint id;
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

void loadUniformf(Uniformf *u);

bool isInBounds(f32 x, f32 y, f32 *dims);


#endif
