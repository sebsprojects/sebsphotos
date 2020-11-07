attribute vec2 in_position;
attribute vec2 in_texcoord;

uniform vec2 res;
uniform vec4 textDims; // xoffs, yoffs, textSize, fontSize (from texture)

varying vec2 texCoord;
varying vec2 texelSize;

void main() {
  texCoord = in_texcoord;
  texelSize = 1.0 / res;
  vec2 pos = (in_position * (textDims.z / textDims.w) + textDims.xy) / res;
  gl_Position = vec4(2.0 * pos - vec2(1.0), 0.0, 1.0);
}
