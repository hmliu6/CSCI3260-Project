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

uniform vec3 lightPosition_1;
uniform vec3 lightPosition_2;


uniform sampler2D displacementMap;
uniform bool displacementMapFlag;

out vec2 UV;
out vec3 worldPos;
out vec3 cameraEye;
out vec3 lightDirection_1;
out vec3 lightDirection_2;
out vec3 cameraNormal;

void main()
{
  if(displacementMapFlag){
    dv = texture2D( displacementMap, vertexUV );
    position = 0.26*dv.x + 0.69*dv.y + 0.15*dv.z;
  }

  vec4 v = vec4(position, 1.0f);


  vec4 new_position = PVM * RotationMatrix * TransformMatrix * ScalingMatrix * v;
	gl_Position = new_position;

  vec4 positionRTS = RotationMatrix * TransformMatrix * ScalingMatrix * v;

  UV = vertexUV;

  // Calculation for lighting
  worldPos = (modelMatrix * positionRTS).xyz;

  vec3 cameraPos = (viewMatrix * modelMatrix * positionRTS).xyz;
  cameraEye = vec3(0.0f, 0.0f, 0.0f) - cameraPos;

  vec3 cameraLight_1 = (viewMatrix * vec4(lightPosition_1, 1.0f)).xyz;
  lightDirection_1 = cameraLight_1 + cameraEye;

  vec3 cameraLight_2 = (viewMatrix * vec4(lightPosition_2, 1.0f)).xyz;
  lightDirection_2 = cameraLight_2 + cameraEye;

  cameraNormal = (viewMatrix * modelMatrix * RotationMatrix * TransformMatrix * ScalingMatrix * vec4(normal, 0.0f)).xyz;

  // Plane rendering for bump mapping
  // if(planeRender == 1.0f){
  //   vec3 cameraTangent = viewModelMatrix * normalize(tangent);
  //   vec3 cameraBitangent = viewModelMatrix * normalize(bitangent);
  //   vec3 cameraNormal = viewModelMatrix * normalize(normal);
  //   mat3 TBN = transpose(mat3(cameraTangent, cameraBitangent, cameraNormal));
  //
  //   lightTangent = TBN * lightDirection_1;
  //   eyeTangent = TBN * cameraEye;
  // }

}
