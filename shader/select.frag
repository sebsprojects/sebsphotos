precision highp float;

uniform vec4 selInnerCoord;

void main()
{
  vec3 fragColor;
  fragColor = vec3(gl_FragCoord.xyz);
  gl_FragColor = vec4(fragColor.xyz, 0.5);
}

