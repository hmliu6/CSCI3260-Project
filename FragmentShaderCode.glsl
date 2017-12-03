#version 410

in vec2 UV;
in vec3 worldPos;
in vec3 cameraEye;
in vec3 lightDirection;
in vec3 cameraNormal;

out vec3 color;


uniform sampler2D myTextureSampler_1;
uniform sampler2D normalMap;
uniform sampler2D myTextureSampler_2;
uniform bool normalMapFlag;
uniform bool secondTextureFlag;

uniform vec3 lightPosition;
uniform float specularCoefficient;
uniform float diffuseCoefficient;

vec3 diffuseLight, ambientLight, specularLight;
float ambientCoefficient = 0.5f;

void main(){
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  color = vec3(0.0f, 0.0f, 0.0f);

  diffuseLight = texture( myTextureSampler_1, UV ).rgb;
  if(secondTextureFlag){
    diffuseLight = 0.5 * texture( myTextureSampler_1, UV ).rgb + 0.5 * texture( myTextureSampler_2, UV ).rgb;
  }

  ambientLight = vec3(ambientCoefficient, ambientCoefficient, ambientCoefficient) * diffuseLight;
  specularLight = vec3(specularCoefficient, specularCoefficient, specularCoefficient);

  float lightDistance = length(lightPosition - worldPos);
  float distanceParams = lightDistance * lightDistance;

  // For bump mapping
  // if(planeRender == 1.0f){
  //   vec3 tangentTexture = normalize(texture(textureNormal, vec2(UV.x, -UV.y)).rgb * 2.0f - 1.0f);
  //   vec3 N = tangentTexture;
  //   vec3 L = normalize(lightTangent);
  //   float cosTheta = clamp(dot(N, L), 0, 1);
  //
  //   vec3 V = normalize(eyeTangent);
  //   vec3 R = reflect(-L, N);
  //   float cosAlpha = clamp(dot(V, R), 0, 1);
  //
  //   color = diffuseLight * lightColor * diffuseCoefficient * cosTheta / (lightDistance * lightDistance)
  //           + specularLight * lightColor * 50.0f * pow(cosAlpha, 100) / (lightDistance * lightDistance);
  // }

  vec3 N = normalize(cameraNormal);
  vec3 L = normalize(lightDirection);

  if(normalMapFlag){
    N = texture(normalMap, UV).rgb;
    N = normalize(N * 2.0f);
  }

  float cosTheta = clamp(dot(N, L), 0, 1);

  vec3 V = normalize(cameraEye);
  vec3 R = reflect(-L, N);
  float cosAlpha = clamp(dot(V, R), 0, 1);

  color += ambientLight + diffuseLight * lightColor * diffuseCoefficient * cosTheta / distanceParams
          + specularLight * lightColor * 20.0f * pow(cosAlpha, 10) / distanceParams;
}
