precision highp float;

uniform sampler2D tex;

uniform vec4 texDim;
uniform vec2 zoomCenter;
uniform float selClip;
uniform vec4 selBounds;

varying vec2 texCoord;

void main()
{
  vec3 fragColor;
  vec2 lb;
  vec2 ub;
  if(selClip >= 0.0) {
    lb = selBounds.xy;
    ub = selBounds.zw;
  } else {
    lb = vec2(0.0);
    ub = texDim.zw;
  }
  if(texCoord.x < lb.x || texCoord.y < lb.y ||
     texCoord.x > ub.x || texCoord.y > ub.y) {
    fragColor = vec3(0.15);
  } else {
    fragColor = texture2D(tex, texCoord / texDim.xy).xyz;
  }
  gl_FragColor = vec4(fragColor.xyz, 1.0);
}

