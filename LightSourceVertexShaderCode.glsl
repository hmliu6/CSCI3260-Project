#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 ScalingMatrix;
uniform mat4 TransformMatrix;
uniform mat4 RotationMatrix;

uniform mat4 PVM;
uniform mat3 viewModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightPosition;

out vec2 UV;
out vec3 worldPos;
out vec3 cameraEye;
out vec3 lightDirection;
out vec3 cameraNormal;

void main(){
  vec4 v = vec4(position, 1.0f);
  vec4 new_position = PVM * RotationMatrix * TransformMatrix * ScalingMatrix * v;
	gl_Position = new_position;

  UV = vertexUV;

  // Calculation for lighting
  worldPos = (modelMatrix * v).xyz;

  vec3 cameraPos = (viewMatrix * modelMatrix * v).xyz;
  lightDirection = vec3(0.0f, 0.0f, 0.0f) - cameraPos;

  cameraNormal = (viewMatrix * modelMatrix * vec4(normal, 0.0f)).xyz;

}
