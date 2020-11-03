precision highp float;

uniform float time;
uniform vec4 selCoord;

bool inRange(in float v, in float l, in float u)
{
  return v >= l && v < u;
}

float sdBox(in vec2 p, in vec2 l)
{
  vec2 d = abs(p) - l;
  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// continuously map the outline of the box to [0, 2 * w + 2 * h]
// counter-clockwise, starting at
float mapBoxToLine(in vec2 p)
{
  vec2 a = p - selCoord.xy;
  vec2 b = p - selCoord.zw;
  vec2 wh = selCoord.zw - selCoord.xy;
  float v = 0.0;
  if(abs(a.x) < 1.0 && a.y > -1.0 && b.y < 0.0) {
    v = wh.y - a.y;
  } else if(abs(a.y) < 1.0 && a.x > 0.0 && b.x < 1.0) {
    v = wh.y + a.x;
  } else if(abs(b.x) < 1.0 && a.y > 0.0 && b.y < 1.0) {
    v = wh.x + wh.y + a.y;
  } else if(abs(b.y) < 1.0 && a.x > -1.0 && b.x < 0.0) {
    v = wh.x + 2.0 * wh.y + wh.x - a.x;
  }
  return v;
}

// TODO: Calculate how many segments with pw, then find a fractional with
// close to pw such that we get only full segments
float boxPattern(in float v, in float total)
{
  float pw = 8.0;
  float cnt = total / pw;
  float rem = mod(total, pw);
  pw += rem / cnt;
  float m = mod(v, pw);
  return m < pw * 0.5 ? 0.2 : 0.8;
}

void main()
{
  vec2 p = gl_FragCoord.xy;
  vec2 xy = selCoord.xy;
  vec2 wh = selCoord.zw - selCoord.xy;
  float v = sdBox(p - (xy + 0.5 * wh), 0.5 * wh);
  float x = 2.0 * (wh.x + wh.y);
  float t = -time * 30.0;
  if(inRange(v, 0.0, 1.0)) {
    //vec3 col = vec3(mapBoxToLine(p) / x);
    vec3 col = vec3(boxPattern(mapBoxToLine(p) + t, x));
    //vec3 col = vec3(1.0);
    gl_FragColor = vec4(col, 1.0);
  } else if(inRange(v, 1.0, 2.0)) {
    gl_FragColor = vec4(1.0);
  } else {
    discard;
  }
}

