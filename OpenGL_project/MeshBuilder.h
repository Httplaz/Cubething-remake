#pragma once
#include "Chunk.h"
#include "Vertexpool.h"
#include "BoxList.h"
class MeshBuilder
{
	struct Vertex
	{
		int posX, posY, posZ;
	};
	struct MeshAttribPack
	{
		int side;
		int chunkPosX, chunkPosY, chunkPosZ;
	};
public:
	static void buildMesh(Vertexpool* vertexpool, Chunk* chunk, Chunk** neighbors);
	static void destroyMesh(Vertexpool* vertexpool, Chunk* chunk);
	static Vertex* vertexOffset(int x, int y, int z, Vertex* sample, int vertexCount);
private:
	const static Vertex sides[6][6]; //right up back left down front
	const static ivec3 sideDirections[6];
};

