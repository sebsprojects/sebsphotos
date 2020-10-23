precision highp float;

uniform vec3 iResolution;
uniform float iTime;

uniform sampler2D tex;

varying vec2 texCoord;

void main()
{
  vec3 fragColor;
  //fragColor = vec3(texCoord.xy, 0.0);
  fragColor = texture2D(tex, texCoord).xyz;
  gl_FragColor = vec4(fragColor.xyz, 1.0);
}
