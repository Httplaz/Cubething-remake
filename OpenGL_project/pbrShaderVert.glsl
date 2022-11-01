#version 430 core
layout (location = 0) in uint info;
layout (location = 1) in uint texID0;
layout (location = 2) in uint blockOcclusion0;

layout (location = 3) in int aSide;
layout (location = 4) in ivec3 chunkPos;

out ivec3 WorldPos0;
out ivec2 Length;
out int side;
out int texID1;
out int blockAO1;

uniform ivec3 loadOffset;
uniform ivec3 loadSide; 
void main()
{
    texID1 = int(texID0);
    blockAO1 = int(blockOcclusion0);
    side = aSide;
    ivec3 pos;
    uint info0 = info;

    pos = ivec3((info0) & ((1<<6) -1), (info0>>6) & ((1<<6) -1), (info0>>12) & ((1<<6) -1));
    Length = ivec2((info0 >> 18) & ((1<<6) -1), (info0 >> 24) & ((1<<6) -1));

    WorldPos0 = ivec3(vec4(pos+(ivec3(chunkPos/32+loadOffset+loadSide)%loadSide)*32, 1.0));
    gl_Position = vec4(WorldPos0, 1.0);
}