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
	ChunkHolder(const uint32_t loadSide, vec3 loaderPos, Vertexpool* pool);
	ChunkHolder(const ivec3 loadSide, vec3 loaderPos, Vertexpool* pool);
	ivec3 getChunkOffset();

	void finish();

	static void handleChunks(ChunkHolder* holder);

	const ivec3 loadSide;
	const uint32_t loadAreaZ = loadSide.x*loadSide.y, loadAreaY = loadSide.x * loadSide.z;
	const uint32_t loadVolume = loadSide.y*loadAreaY;
private:
	bool workFinished;
	set<Chunk*> toUpdate;
	vector<Chunk*> toRebuild;
	Vertexpool* vertexpool;
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

