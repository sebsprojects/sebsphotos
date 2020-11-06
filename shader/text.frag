precision highp float;

uniform sampler2D tex;

varying vec2 texCoord;
varying vec2 texelSize;

void main()
{
  float current = texture2D(tex, texCoord).r;
  float dx = texture2D(tex, texCoord + vec2(texelSize.x, 0.0)).r;
  float dy = texture2D(tex, texCoord + vec2(0.0, texelSize.y)).r;
  float fw = abs(dx) + abs(dy);
  float scale = 1.0 / fw;
  float sd = (current - 0.5) * scale;
  float color = clamp(sd + 0.7, 0.0, 1.0);
  float alpha = clamp(sd + 0.7 + 0.0125 * scale, 0.0, 1.0);
  //gl_FragColor = vec4(1.0);
  //gl_FragColor = vec4(color, 1.0, 1.0, 1.0);
  gl_FragColor = vec4(color, color, color, alpha);
}

