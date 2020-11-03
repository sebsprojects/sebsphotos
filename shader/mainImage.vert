attribute vec2 in_position;
attribute vec2 in_texcoord;

uniform vec2 texDim;
uniform vec2 zoomCenter;

varying vec2 texCoord;

void main() {
  texCoord = in_texcoord;
  gl_Position = vec4(in_position, 0.0, 1.0);
}
