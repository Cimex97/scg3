/**
 * \file skybox_frag.glsl
 * \brief Skybox fragment shader.
 */

#version 150

smooth in vec3 texCoord0;

uniform samplerCube texture0;

uniform samplerCube texture1;

uniform float blendFactor;

uniform mat4 colorMatrix;

out vec4 fragColor;


void main() {
  vec4 color1 = texture(texture0, texCoord0);
  vec4 color2 = texture(texture1, texCoord0);
  vec4 finalColor = mix(color1,color2,blendFactor);
  
  // transform color by color matrix
  vec4 transformedColor = colorMatrix * vec4(finalColor.rgb, 1.);
  transformedColor /= transformedColor.a;   // perspective division
    
  // set final fragment color
  fragColor = clamp(vec4(transformedColor.rgb, finalColor.a), 0., 1.);
}
