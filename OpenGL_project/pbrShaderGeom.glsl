#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoords0[];
in vec3 WorldPos0[];
in mat3 TBN0[];

out vec2 TexCoords;
out vec3 WorldPos;
out mat3 TBN;

void copyVertex(vec4 position)
{    
    gl_Position = position;
    EmitVertex();   
}

void main() 
{    
   TexCoords = TexCoords0[0];
   WorldPos = WorldPos0[0];
   TBN = TBN0[0];
   copyVertex(gl_in[0].gl_Position);
   TexCoords = TexCoords0[1];
   WorldPos = WorldPos0[1];
   TBN = TBN0[1];
   copyVertex(gl_in[1].gl_Position);
   TexCoords = TexCoords0[2];
   WorldPos = WorldPos0[2];
   TBN = TBN0[2];
   copyVertex(gl_in[2].gl_Position);
   EndPrimitive();
}  