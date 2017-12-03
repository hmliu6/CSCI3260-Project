#version 440
in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location=2) vec3 normal;
in layout (location = 3) in mat4 instanceMatrix;

uniform mat4 MVP;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPositionWorld;


out vec2 uv;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = MVP * instanceMatrix * vec4(position, 1.0);
    uv = vertexUV;
}
