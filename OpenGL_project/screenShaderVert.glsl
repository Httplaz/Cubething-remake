#version 430 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 texturePosition;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoords;

void main()
{
    texCoords = texturePosition;
    gl_Position = projection*view*model*vec4(vertexPosition, 1);
    //gl_Position = projection*vec4(vertexPosition, 1);
}