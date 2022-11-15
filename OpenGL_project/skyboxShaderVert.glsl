#version 430 core
layout (location = 0) in vec3 vertexPosition;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = vertexPosition;
    gl_Position = (projection * view * vec4(vertexPosition, 1.0)).xyzw;
}  