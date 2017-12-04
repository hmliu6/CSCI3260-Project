#version 410

in vec2 UV;
in vec3 worldPos;
in vec3 cameraEye;
in vec3 lightDirection;
in vec3 cameraNormal;

out vec3 color;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;
uniform bool normalMapFlag;

uniform vec3 lightPosition;
uniform float specularCoefficient;
uniform float diffuseCoefficient;
uniform int fogFlag;
uniform float fogDensity;
uniform float fogGradient;
uniform vec3 fogColor;

vec3 diffuseLight, ambientLight, specularLight;
float ambientCoefficient = 1.7f;


void main(){
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  color = vec3(0.0f, 0.0f, 0.0f);

  diffuseLight = texture( colorTexture, UV ).rgb;
  ambientLight = vec3(ambientCoefficient, ambientCoefficient, ambientCoefficient) * diffuseLight;

  color = ambientLight;

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
