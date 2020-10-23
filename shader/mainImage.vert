attribute vec4 in_position;
attribute vec2 in_texCoord;

varying vec2 texCoord;

void main() {
  texCoord = in_texCoord;
  gl_Position = in_position;
}
