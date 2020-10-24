attribute vec4 in_position;
attribute vec2 in_texCoord;

uniform vec2 texDim;

varying vec2 texCoord;

void main() {
  texCoord = in_texCoord / texDim;
  gl_Position = in_position;
}
