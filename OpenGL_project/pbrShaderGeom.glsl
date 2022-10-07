#version 430 core


ivec3 normal[6] = {ivec3(1,0,0), ivec3(0,1,0), ivec3(0,0,1), ivec3(-1,0,0), ivec3(0,-1,0), ivec3(0,0,-1)};
ivec3 tangent[6] = {ivec3(0,0,1), ivec3(1,0,0), ivec3(-1,0,0), ivec3(0,0,-1), ivec3(1,0,0), ivec3(1,0,0)};
ivec3 bitangent[6] = {ivec3(0,1,0), ivec3(0,0,1), ivec3(0,1,0), ivec3(0,1,0), ivec3(0,0,-1), ivec3(0,1,0)};


layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in ivec3 WorldPos0[];
in ivec2 Length[];
in int side[];
in int texID1[];


uniform mat4 transform;

out vec2 TexCoords;
out vec3 WorldPos;
out mat3 TBN;
out flat int side1;
out flat int texID;

void copyVertex(ivec3 position, ivec3 local)
{    

    int rep = side[0]%3;
    TexCoords = rep==0? local.zy*1. : rep==1?local.xz*1. : local.xy*1.;
    WorldPos = position.xyz;
    gl_Position = transform*ivec4(position, 1.);
    side1 = side[0];
    texID = texID1[0];
    EmitVertex();   
}

void main() 
{    
   TBN = mat3(tangent[side[0]], bitangent[side[0]], normal[side[0]]);
   ivec3 deltaRight = tangent[side[0]] * Length[0].x;
   ivec3 deltaUp = bitangent[side[0]] * Length[0].y;
   copyVertex((WorldPos0[0]+deltaRight+deltaUp), deltaRight+deltaUp);
   copyVertex((WorldPos0[0]+deltaUp), deltaUp);
   copyVertex((WorldPos0[0]+deltaRight), deltaRight);

   copyVertex(WorldPos0[0], ivec3(0));
   EndPrimitive();
}  