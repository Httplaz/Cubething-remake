#pragma once
#include "Chunk.h"
#include "Vertexpool.h"
#include "BoxList.h"
#include "RingBuffer3.h"
#include <glm/gtx/rotate_vector.hpp>

struct CompactVertex
{
	uint32_t rectData;
	uint32_t texID;
	uint32_t infoAdv;
};
struct MeshAttribPack
{
	int side;
	int chunkPosX, chunkPosY, chunkPosZ;
};
class MeshBuilder
{
	struct Vertex
	{
		uint32_t posX, posY, posZ;
		uint32_t lengthX, lengthZ;
		uint32_t texID;
		uint32_t infoAdv;
		//int posX1, posY1, posZ1;
		//int posX2, posY2, posZ2;
		//int posX3, posY3, posZ3;
	};
public:
	static void buildMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk, RingBuffer3<Chunk*> ring, bool restart);
	static void disableMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk);
	static void enableMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk);
	static void destroyMesh(Vertexpool<CompactVertex, MeshAttribPack>* vertexpool, Chunk* chunk);
	static Vertex* vertexOffset(int x, int y, int z, Vertex* sample, int vertexCount);
private:
	static mat3 rightRotationMatrix(ivec3 axis);
	const static Vertex sides[6][6]; //right up back left down front
	const static ivec3 sideDirections[6];
};

