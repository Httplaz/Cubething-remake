#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSide;
layout (location = 2) in vec3 chunkPos;

out vec2 TexCoords0;
out vec3 WorldPos0;
out mat3 TBN0;

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
            TexCoords0 = (worldPos.zy);
            break;
        case 1:
            TexCoords0 = (worldPos.xz);
            break;
        case 2:
            TexCoords0 = (worldPos.xy);
            break;
        case 3:
            TexCoords0 = (worldPos.zy);
            break;
        case 4:
            TexCoords0 = (worldPos.xz);
            break;
        case 5:
            TexCoords0 = (worldPos.xy);
            break;
    }

    WorldPos0 = worldPos;
    TBN0 = mat3(tangent[side], bitangent[side], normal[side]);
    gl_Position = projection * view * vec4(WorldPos0, 1.0);
}