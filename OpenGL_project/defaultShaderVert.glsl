#version 430 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in float aSide;
layout (location = 2) in vec3 chunkPos;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main()
{
    gl_Position = projection*view*model*vec4(vertexPosition+chunkPos, 1.0)+vec4(0,0,0.0001, 0.);
}