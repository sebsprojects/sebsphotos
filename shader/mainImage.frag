precision highp float;

// uniform sampler2D tex;

varying vec2 tex_coord;

void main()
{
  vec3 fragColor;
  fragColor = vec3(tex_coord.xy, 0.0);
  //fragColor = texture2D(tex, texCoord).xyz;
  //fragColor = vec3(gl_FragCoord.x / 640.0, 0.0, 0.0);
  gl_FragColor = vec4(fragColor.xyz, 1.0);
}
