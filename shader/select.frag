precision highp float;

uniform vec4 selCoord;

float sdBox(in vec2 p, in vec2 l)
{
  vec2 d = abs(p) - l;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

float sdConv(in float v)
{
  return abs(sign(v) - 1.0) * 0.5;
}

// continuously map the outline of the box to [0, 2 * w + 2 * h]
// counter-clockwise, starting at
float mapBoxToLine(in vec2 p)
{
  vec2 a = p - selCoord.xy;
  vec2 b = p - selCoord.zw;
  vec2 wh = selCoord.zw - selCoord.xy;
  float v = 0.0;
  if(abs(a.x) <= 1.0) {
    v = wh.y - a.y;
  } else if(abs(a.y) <= 1.0) {
    v = wh.y + a.x;
  } else if(abs(b.x) <= 1.0) {
    v = wh.x + wh.y + a.y;
  } else if(abs(b.y) <= 1.0) {
    v = 2.0 * wh.x + wh.y + wh.x - a.x;
  }
  return v;
}

// TODO: Calculate how many segments with pw, then find a fractional with
// close to pw such that we get only full segments
float boxPattern(in float v)
{
  float pw = 4.0;
  float m = mod(v, pw * 2.0);
  return m < pw ? 0.0 : 1.0;
}

void main()
{
  vec2 p = gl_FragCoord.xy;
  vec2 wh = (selCoord.zw - selCoord.xy);
  float v = sdBox(p - (selCoord.xy + 0.5 * wh), 0.5 * wh);
  if(v >= 0.0 && v <= 1.0) {
    gl_FragColor = vec4(vec3(boxPattern(mapBoxToLine(p))), 1.0);
  } else {
    discard;
  }
}

