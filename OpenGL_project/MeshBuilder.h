#pragma once
#include "Chunk.h"
#include "Vertexpool.h"
#include "BoxList.h"
class MeshBuilder
{
	struct Vertex
	{
		uint32_t posX, posY, posZ;
		uint32_t lengthX, lengthZ;
		uint32_t texID;
		//uint8_t AO;
		//int posX1, posY1, posZ1;
		//int posX2, posY2, posZ2;
		//int posX3, posY3, posZ3;
	};
	struct CompactVertex
	{
		uint32_t rectData;
		uint32_t texID;
		//uint8_t AO;
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

