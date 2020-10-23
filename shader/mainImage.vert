attribute vec4 in_position;
attribute vec2 in_texCoord;

varying vec2 tex_coord;

void main() {
  tex_coord = in_texCoord / 1000.0;
  gl_Position = in_position;
}
