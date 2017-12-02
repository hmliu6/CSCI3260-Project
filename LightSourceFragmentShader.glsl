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

vec3 diffuseLight, ambientLight, specularLight;
float ambientCoefficient = 100.0f;

void main(){
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  color = vec3(0.0f, 0.0f, 0.0f);

  diffuseLight = texture( colorTexture, UV ).rgb + 10000.0f;
  ambientLight = vec3(ambientCoefficient, ambientCoefficient, ambientCoefficient) * diffuseLight;

  color = ambientLight;
}
