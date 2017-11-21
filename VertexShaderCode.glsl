/*
	CSCI3260 Assignment 2
	Name : Yau Yui Pan
	Student ID : 1155081383
*/
#version 440  // GLSL version your computer supports

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location=2) vec3 normal;


uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPositionWorld;

out vec2 UV;

out vec3 worldPosition;
out vec3 cameraNormalSpace;
out vec3 eyeDirection;
out vec3 lightDirection;

//out vec3 normalWorld;
//out vec3 vertexPositionWorld;

void main()
{
	vec4 v = vec4(position, 1.0);

	vec4 new_position =  MVP  * v;
	
	gl_Position = new_position;	
    UV = vertexUV;

	worldPosition = (modelMatrix * v).xyz;

	vec3 vertexPositionCameraspace = ( viewMatrix * modelMatrix * v).xyz;
	eyeDirection = vec3(0,0,0) - vertexPositionCameraspace;

	vec3 lightDirection = ( viewMatrix * vec4(lightPositionWorld,1)).xyz;
	lightDirection = lightDirection + eyeDirection;

	cameraNormalSpace = ( viewMatrix * modelMatrix * vec4(normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

}