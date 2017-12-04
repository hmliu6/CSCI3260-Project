#version 410

in vec3 textureCoords;


uniform int fogFlag;
uniform float fogDensity;
uniform float fogGradient;
uniform vec3 fogColorVec3;

vec4 fogColor;

out vec4 color;

uniform samplerCube skybox;

void main(){
    color = texture(skybox, textureCoords);
    // color = vec4(textureCoords, 1.0f);

    fogColor = vec4(fogColorVec3, 1.0f);
    if(fogFlag == 1){
      //float distance = length((viewMatrix * RotationMatrix * TransformMatrix * ScalingMatrix * v));
      float visibility = 1;
      float distance_of_eye_to_skybox = 85.0f;
      visibility = exp(-pow((distance_of_eye_to_skybox * fogDensity), fogGradient));
      visibility = clamp(visibility, 0, 1);
      color = mix(fogColor, color, visibility);
    }else{
      color = mix(fogColor, color, 1);
    }

}
