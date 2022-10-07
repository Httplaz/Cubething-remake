#pragma once
#include <glm/glm.hpp>
#include "BoxList.h"
#include "RingBuffer3.h"
#include "Chunk.h"
#include "Vertexpool.h"
#include "WorldGenerator.h"
#include "MeshBuilder.h"
#include <set>
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
	ChunkHolder(const unsigned loadCubeSide, vec3 loaderPos, Vertexpool* pool);
	ivec3 getChunkOffset();

	const unsigned loadCubeSide;
	const unsigned loadCubeArea;
	const unsigned loadCubeVolume;
private:
	Vertexpool* vertexpool;
	ivec3 chunkOffset;
	RingBuffer3<Chunk*> chunkRing;
	set<chunkTask> chunkTasks;
	Chunk** prepareChunkArray();
	unsigned nextChunkID;
	Chunk* loadChunk(ivec3 pos);
	void replaceChunk(ivec3 pos);
	void integrateChunk(ivec3 pos);
	void integrateChunk(Chunk* chunk);
	void unloadChunk(Chunk* chunk);
	unsigned chunkIndex(ivec3 chunkPos);

};

