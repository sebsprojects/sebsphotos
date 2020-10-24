precision highp float;

uniform vec2 texDim;
uniform sampler2D tex;

varying vec2 texCoord;

void main()
{
  vec3 fragColor;
  //fragColor = vec3(texCoord.xy, 0.0);
  fragColor = texture2D(tex, texCoord).xyz;
  //fragColor = vec3(gl_FragCoord.x / 640.0, 0.0, 0.0);
  gl_FragColor = vec4(fragColor.xyz, 1.0);
}
