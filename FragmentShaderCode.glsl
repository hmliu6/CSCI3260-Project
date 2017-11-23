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
float ambientCoefficient = 0.5f;

void main(){
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  color = vec3(0.0f, 0.0f, 0.0f);
  
  

  
  diffuseLight = texture( colorTexture, UV ).rgb;
  ambientLight = vec3(ambientCoefficient, ambientCoefficient, ambientCoefficient) * diffuseLight;
  specularLight = vec3(specularCoefficient, specularCoefficient, specularCoefficient);

  float lightDistance = length(lightPosition - worldPos);



  vec3 N = normalize(cameraNormal);
  vec3 L = normalize(lightDirection);
  // For bump mapping
  if(normalMapFlag){
	//vec3 normal = normalize(worldPos);
	N = texture(normalMap, UV).rgb;
	N = normalize(N*2.0 -1.0);
  }
  float cosTheta = clamp(dot(N, L), 0, 1);

  vec3 V = normalize(cameraEye);
  vec3 R = reflect(-L, N);
  float cosAlpha = clamp(dot(V, R), 0, 1);

  color += ambientLight
		  + diffuseLight * lightColor * diffuseCoefficient * cosTheta / (lightDistance * lightDistance)
          + specularLight * lightColor * 50.0f * pow(cosAlpha, 50) / (lightDistance * lightDistance);
}
