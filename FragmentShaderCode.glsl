/*
	CSCI3260 Assignment 2
	Name : Yau Yui Pan
	Student ID : 1155081383
*/
#version 440 //GLSL version your computer supports


// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec3 ambientLight;
uniform vec3 diffuseLightColor;
uniform vec3 specularLightColor;
uniform vec3 lightPositionWorld;
uniform vec3 eyePositionWorld;

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 worldPosition;
in vec3 cameraNormalSpace;
in vec3 eyeDirection;
in vec3 lightDirection;


void main(){

	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;

	vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb ;
	vec3 MaterialAmbientColor = ambientLight * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = specularLightColor;
	
	float distance = length( lightPositionWorld - worldPosition );
	
	vec3 n = normalize(cameraNormalSpace);
	vec3 l = normalize( lightPositionWorld );
	

	float cosTheta = clamp( dot( l, n), 0,1 );

	vec3 E = normalize(eyeDirection);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	
	float cosAlpha = clamp( dot( E,R ), 0,1 );
	
	color = 
		// ambient
		MaterialAmbientColor +
		// diffuse
		diffuseLightColor * MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// specular
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,15) / (distance*distance);
}