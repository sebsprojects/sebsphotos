precision highp float;

uniform sampler2D tex;

uniform vec2 texDim;
uniform vec2 zoomCenter;

varying vec2 texCoord;

void main()
{
  vec3 fragColor;
  if(texCoord.x < 0.0 || texCoord.y < 0.0 ||
     texCoord.x > texDim.x || texCoord.y > texDim.y) {
    fragColor = vec3(0.0);
  } else {
    fragColor = texture2D(tex, texCoord / texDim).xyz;
  }
  if(length(texCoord - zoomCenter) < 50.0) {
    fragColor = vec3(1.0, 0.0, 0.0);
  }
  gl_FragColor = vec4(fragColor.xyz, 1.0);
}

//fragColor = vec3(texCoord.xy, 0.0);
//fragColor = vec3(gl_FragCoord.x / 640.0, 0.0, 0.0);
