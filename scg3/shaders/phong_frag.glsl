/**
 * \file phong_frag.glsl
 * \brief Phong fragment shader, uses external functions applyLighting()
 *    and applyTexture().
 */

#version 150

smooth in vec3 ecVertex;
smooth in vec3 ecNormal;
smooth in vec4 texCoord0;
smooth in vec4 ShadowCoord;

uniform mat4 colorMatrix;
uniform mat4 depthMVP;
uniform sampler2D texture0;
uniform sampler2D ShadowMap;
uniform vec3 lightDir;

out vec4 fragColor;


// --- declarations ---


void applyLighting(const in vec3 ecVertex, const in vec3 ecNormal,
    out vec4 emissionAmbientDiffuse, out vec4 specular, in float visibility);

vec4 applyTexture(const in vec4 texCoord, const in vec4 emissionAmbientDiffuse,
    const in vec4 specular);


// --- implementations ---


void main(void) {

    float bias = 0.005;
    vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
    float visibility = 1.0;
    float closestDepth = texture(ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    visibility = currentDepth - bias < closestDepth ? 1.0 : 0.5;
    if (ShadowCoord.z > 1.0) {
       visibility = 1.0;
    }
  // apply lighting model (to be defined in separate shader)
  vec4 emissionAmbientDiffuse, specular;
  applyLighting(ecVertex, ecNormal, emissionAmbientDiffuse, specular, visibility);

  // apply texture and determine color (to be defined in separate shader)
  vec4 color = applyTexture(texCoord0, emissionAmbientDiffuse, specular);

  // transform color by color matrix
  vec4 transformedColor = colorMatrix * vec4(color.rgb, 1.);
  transformedColor /= transformedColor.a;   // perspective division

  // set final fragment color
  fragColor = clamp(vec4(transformedColor.rgb, color.a), 0., 1.);
}
