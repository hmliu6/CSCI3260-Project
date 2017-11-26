#version 410

layout(location = 0) in vec3 position;

out vec3 textureCoords;

uniform mat4 ScalingMatrix;
uniform mat4 TransformMatrix;
uniform mat4 RotationMatrix;

uniform mat4 PVM;

void main(){
    vec4 v = vec4(position, 1.0f);
    vec4 new_position = PVM * RotationMatrix * TransformMatrix * ScalingMatrix * v;
    new_position.w = new_position.z + 0.01f;
	gl_Position = new_position;
    
    textureCoords = vec3(-position.x, -position.y, -position.z);
}