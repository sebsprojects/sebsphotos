attribute vec2 in_position;

uniform vec4 selCoord;

void main()
{
  gl_Position = vec4(in_position, 0.0, 1.0);
}
