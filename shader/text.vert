attribute vec2 in_position;
attribute vec2 in_texcoord;

uniform vec2 res;

varying vec2 texCoord;
varying vec2 texelSize;

void main() {
  texCoord = in_texcoord;
  texelSize = 1.0 / res;
  gl_Position = vec4(0.5 * in_position / res - vec2(1.0), 0.0, 1.0);
}
