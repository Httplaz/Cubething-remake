#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSide;
layout (location = 2) in vec3 chunkPos;

out vec2 TexCoords;
out vec3 WorldPos;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform ivec4 offsetInfo;

//right up back left down front

vec3 normal[6] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(-1,0,0), vec3(0,-1,0), vec3(0,0,-1)};
vec3 tangent[6] = {vec3(0,0,1), vec3(1,0,0), vec3(-1,0,0), vec3(0,0,-1), vec3(-1,0,0), vec3(1,0,0)};
vec3 bitangent[6] = {vec3(0,1,0), vec3(0,0,1), vec3(0,1,0), vec3(0,1,0), vec3(0,0,-1), vec3(0,1,0)};


void main()
{
    int side = int(aSide);
    vec3 worldPos = vec3(model * vec4(aPos+(ivec3(chunkPos/32+offsetInfo.xyz+offsetInfo.www)%offsetInfo.w)*32, 1.0));
    switch(side)
    {
        case 0:
            TexCoords = (worldPos.zy);
            break;
        case 1:
            TexCoords = (worldPos.xz);
            break;
        case 2:
            TexCoords = (worldPos.xy);
            break;
        case 3:
            TexCoords = (worldPos.zy);
            break;
        case 4:
            TexCoords = (worldPos.xz);
            break;
        case 5:
            TexCoords = (worldPos.xy);
            break;
    }

    WorldPos = worldPos;
    TBN = mat3(tangent[side], bitangent[side], normal[side]);
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}