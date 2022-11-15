#version 430 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 offset;
uniform mat4 transform;

void main()
{
    gl_Position = (transform*vec4(vertexPosition+offset, 1.0));
}  