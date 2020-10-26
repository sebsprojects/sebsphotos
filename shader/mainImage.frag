precision highp float;

uniform vec2 texDim;
uniform sampler2D tex;

varying vec2 texCoord;

void main()
{
  vec3 fragColor;
  if(texCoord.x < 0.0 || texCoord.y < 0.0 ||
     texCoord.x > 1.0 || texCoord.y > 1.0) {
    fragColor = vec3(0.0);
  } else {
    fragColor = texture2D(tex, texCoord).xyz;
  }
  gl_FragColor = vec4(fragColor.xyz, 1.0);
}

//fragColor = vec3(texCoord.xy, 0.0);
//fragColor = vec3(gl_FragCoord.x / 640.0, 0.0, 0.0);
