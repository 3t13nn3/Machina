#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

struct PointLight {
  vec4 position; // ignore w
  vec4 color;    // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projection;
  mat4 view;
  mat4 invView;
  vec4 ambientLightColor; // w is intensity
  PointLight pointLights[10];
  int numLights;
}
ubo;

layout(set = 0, binding = 1) uniform TimeUbo { float timeElapsed; }
timeUbo;

layout(binding = 2) uniform sampler2D mySampler;

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
  vec3 color;
  float dist;
}
push;

vec3 calculateDirectionalLight(vec3 normal, vec3 fragPos, vec3 viewDirection) {
  vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

  vec4 color = vec4(1.0, 0.8, 0.5, .8);
  vec3 intensity = (color.rgb * color.w);

  float cosAngIncidence = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = intensity * cosAngIncidence;

  return diffuse;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection) {
  // DIFF
  // Direction from fragment to light
  vec3 lightDir = light.position.xyz - fragPos;

  float attenuation = 1.0 / dot(lightDir, lightDir);

  // Normalize light direction
  lightDir = normalize(lightDir);

  // Calculate the contribution (Cosinus Angle incidence)
  float cosAngIncidence = max(dot(normal, lightDir), 0.0);
  vec3 intensity = (light.color.rgb * light.color.w) * attenuation;

  vec3 diffuse = intensity * cosAngIncidence;

  // SPEC (blin-phong)
  vec3 halfAngle = normalize(lightDir + viewDirection);
  float blinnTerm = dot(normal, halfAngle);
  blinnTerm = clamp(blinnTerm, 0, 1);
  blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
  vec3 spec = intensity * blinnTerm;

  return diffuse + spec;
}

void main() {

  vec3 surfaceNormal = normalize(fragNormalWorld);
  vec3 cameraPosWorld = ubo.invView[3].xyz;
  vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

  // 3 kind of illuminations
  vec3 ambientLight = ubo.ambientLightColor.rgb * ubo.ambientLightColor.w;
  vec3 diffSpec = vec3(0.0);

  for (int i = 0; i < ubo.numLights; ++i) {
    diffSpec += calculatePointLight(ubo.pointLights[i], surfaceNormal, fragPosWorld, viewDirection);
  }

  diffSpec += calculateDirectionalLight(surfaceNormal, fragPosWorld, viewDirection);

  // Final color
  vec3 finalColor = fragColor * (ambientLight + diffSpec);

  vec3 color = texture(mySampler, vec2(2048)).xyz;

  outColor = vec4(pow(finalColor + color, vec3(0.4545)), clamp(push.dist, 0.0, 1.0));
}
