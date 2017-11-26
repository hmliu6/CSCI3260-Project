#version 410

in vec3 textureCoords;

out vec4 color;

uniform samplerCube skybox;

void main(){
    color = texture(skybox, textureCoords);
    // color = vec4(textureCoords, 1.0f);
}