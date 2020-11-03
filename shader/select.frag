precision highp float;

uniform vec4 selCoord;
uniform float time;


bool inRange(in float x, in float a, in float b)
{
  return x >= a && x < b;
}

float box(in vec2 p, in float t)
{
  vec2 wh = selCoord.zw - selCoord.xy + vec2(2.0 * t);
  vec2 a = p - selCoord.xy + vec2(t);
  float v = -1.0;
  if(inRange(selCoord.x - p.x, 0.0, t) &&
     inRange(p.y, selCoord.y, selCoord.w + t)) {
    v = wh.y - a.y;
  } else if(inRange(selCoord.y - p.y, 0.0, t) &&
            inRange(p.x, selCoord.x - t, selCoord.z)) {
    v = wh.y + a.x;
  } else if(inRange(p.x - selCoord.z, 0.0, t) &&
            inRange(p.y, selCoord.y - t, selCoord.w)) {
    v = wh.x + wh.y + a.y;
  } else if(inRange(p.y - selCoord.w, 0.0, t) &&
            inRange(p.x, selCoord.x, selCoord.z + t)) {
    v = 2.0 * (wh.x + wh.y) - a.x;
  }
  return v;
}

float boxPattern(in float v, in float total)
{
  float pw = 12.0;
  float cnt = total / pw;
  float rem = mod(total, pw);
  pw += rem / cnt;
  float m = mod(v, pw);
  return m < pw * 0.5 ? 0.2 : 0.9;
}

void main()
{
  vec2 p = gl_FragCoord.xy;
  vec2 wh = selCoord.zw - selCoord.xy;
  vec3 col = vec3(0.0);
  float t = 2.0;
  float sum = 2.0 * (wh.x + wh.y + 4.0 * t);
  float v = box(p, t);
  if(v < 0.0) {
    discard;
  }
  col = vec3(boxPattern(v + time * 50.0, sum));
  gl_FragColor = vec4(col, 1.0);
}
