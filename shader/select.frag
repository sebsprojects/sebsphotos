precision highp float;

uniform vec4 selCoord;
uniform vec2 selRes;

void main()
{
  vec3 fragColor;
  fragColor = vec3(gl_FragCoord.xyz);
  if(abs(gl_FragCoord.x - selCoord.x) <= 1.0 ||
     abs(gl_FragCoord.x - selCoord.z) <= 1.0 ||
     abs(gl_FragCoord.y - selCoord.y) <= 1.0 ||
     abs(gl_FragCoord.y - selCoord.w) <= 1.0) {
    gl_FragColor = vec4(1.0);
  } else {
    discard;
  }
}

