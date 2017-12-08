#version 410

in vec2 UV;
in vec3 worldPos;
in vec3 cameraEye;
in vec3 lightDirection_1;
in vec3 lightDirection_2;
in vec3 cameraNormal;

out vec3 color;


uniform sampler2D myTextureSampler_1;
uniform sampler2D normalMap;
uniform sampler2D myTextureSampler_2;
uniform bool normalMapFlag;
uniform bool secondTextureFlag;
uniform bool globalNormalMapFlag;
uniform int fogFlag;
uniform float fogDensity;
uniform float fogGradient;
uniform vec3 fogColor;

uniform vec3 lightPosition_1;
uniform vec3 lightPosition_2;
uniform float specularCoefficient;
uniform float diffuseCoefficient;

vec3 diffuseLight, ambientLight, specularLight;
float ambientCoefficient = 0.5f;


void main(){
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  color = vec3(0.0f, 0.0f, 0.0f);

  diffuseLight = texture( myTextureSampler_1, UV ).rgb;
  if(secondTextureFlag){
    diffuseLight = 0.7 * texture( myTextureSampler_1, UV ).rgb + 0.3 * texture( myTextureSampler_2, UV ).rgb;
  }

  ambientLight = vec3(ambientCoefficient, ambientCoefficient, ambientCoefficient) * diffuseLight;
  specularLight = vec3(specularCoefficient, specularCoefficient, specularCoefficient);

    float lightDistance_1 = length(lightPosition_1 - worldPos);
    float distanceParams_1 = lightDistance_1 * lightDistance_1;

    float lightDistance_2 = length(lightPosition_2 - worldPos);
    float distanceParams_2 = lightDistance_2 * lightDistance_2;

  // For bump mapping
  // if(planeRender == 1.0f){
  //   vec3 tangentTexture = normalize(texture(textureNormal, vec2(UV.x, -UV.y)).rgb * 2.0f - 1.0f);
  //   vec3 N = tangentTexture;
  //   vec3 L = normalize(lightTangent);
  //   float cosTheta_1 = clamp(dot(N, L), 0, 1);
  //
  //   vec3 V = normalize(eyeTangent);
  //   vec3 R = reflect(-L, N);
  //   float cosAlpha = clamp(dot(V, R), 0, 1);
  //
  //   color = diffuseLight * lightColor * diffuseCoefficient * cosTheta_1 / (lightDistance_1 * lightDistance_1)
  //           + specularLight * lightColor * 50.0f * pow(cosAlpha, 100) / (lightDistance_1 * lightDistance_1);
  // }

  vec3 N = normalize(cameraNormal);
  vec3 unitLight_1 = normalize(lightDirection_1);
  vec3 unitLight_2 = normalize(lightDirection_2 );

  if(globalNormalMapFlag && normalMapFlag){
    N = texture(normalMap, UV).rgb;
    N = normalize(N * 2.0f - 1.0f);
  }else{
    N = texture(normalMap, UV).rgb;
    N = normalize(cameraNormal);
  }

  float cosTheta_1 = clamp(dot(N, unitLight_1), 0, 1);
  float cosTheta_2 = clamp(dot(N, unitLight_2), 0, 1);

  vec3 V = normalize(cameraEye);

  vec3 R_1 = reflect(-unitLight_1, N);
  vec3 R_2 = reflect(-unitLight_2, N);
  float cosAlpha_1 = clamp(dot(V, R_1), 0, 1);
  float cosAlpha_2 = clamp(dot(V, R_2), 0, 1);

  color += ambientLight
          + diffuseLight * lightColor * diffuseCoefficient * cosTheta_1 / distanceParams_1
          + diffuseLight * lightColor * diffuseCoefficient * cosTheta_2 / distanceParams_2
          + specularLight * lightColor * 20.0f * pow(cosAlpha_1, 10) / distanceParams_1
          + specularLight * lightColor * 20.0f * pow(cosAlpha_2, 10) / distanceParams_2;

  if(fogFlag == 1){
    float visibility = 1;
    //float distance = length((viewMatrix * RotationMatrix * TransformMatrix * ScalingMatrix * v));
    float distance_of_eye_to_vertex = distance(cameraEye,worldPos);
    visibility = exp(-pow((distance_of_eye_to_vertex * fogDensity), fogGradient));
    visibility = clamp(visibility, 0, 1);
    color = mix(fogColor, color, visibility);
  }else{
    color = mix(fogColor, color, 1);
  }
}
