#pragma once
#include <glm/glm.hpp>
#include "BoxList.h"
#include "RingBuffer3.h"
#include "Chunk.h"
#include "Vertexpool.h"
#include "WorldGenerator.h"
#include "MeshBuilder.h"
#include <set>
#include <thread>
#include "Threadpool.h"
using namespace glm;

struct chunkTask
{
	ivec3 chunkOffset;
	ivec3 chunkPos;
};

class ChunkHolder
{
public:
	ivec3 update(vec3 loaderPos);
	ChunkHolder(const uint32_t loadSide, vec3 loaderPos, Vertexpool<CompactVertex, MeshAttribPack>* pool);
	ChunkHolder(const ivec3 loadSide, vec3 loaderPos, Vertexpool<CompactVertex, MeshAttribPack>* pool);
	uint32_t getBlock(ivec3 pos);
	ivec3 getChunkOffset();
	ivec3 getWorldOffset();
	void setBlock(ivec3 pos, uint32_t block);
	ivec3 getSize();

	void finish();

	const ivec3 loadSide;
	const uint32_t loadAreaZ = loadSide.x*loadSide.y, loadAreaY = loadSide.x * loadSide.z;
	const uint32_t loadVolume = loadSide.y*loadAreaY;
//private:
	struct updaterArgs
	{
		Vertexpool<CompactVertex, MeshAttribPack>* vertexpool;
		ChunkHolder* holder;
		Chunk* chunk;
	};
	Threadpool<1, updaterArgs> threadpool;
	bool workFinished;
	Vertexpool<CompactVertex, MeshAttribPack>* vertexpool;
	ivec3 chunkOffset;
	RingBuffer3<Chunk*> chunkRing;
	set<chunkTask> chunkTasks;
	unsigned nextChunkID;
	Chunk* loadChunk(ivec3 pos);
	void refillChunk(Chunk* chunk);
	void rebuildChunk(Chunk* chunk);
	void unloadChunk(Chunk* chunk);
	unsigned chunkIndex(ivec3 chunkPos);

};

