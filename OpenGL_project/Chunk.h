#pragma once
#define GLFW_INCLUDE_NONE

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;


struct ivec3Ref
{
	int32_t* x;
	int32_t* y;
	int32_t* z;
	ivec3 toIvec3()
	{
		return { (*x), (*y), (*z) };
	}
	void toIvec3(ivec3& ref)
	{
		ref.x = *x;
		ref.y = *y;
		ref.z = *z;
	}
};

class Chunk
{ 
public:
	static const int32_t side = 32;
	static const uint32_t area = side * side;
	static const uint32_t volume = side * side * side;
	static uint32_t dataIndex(uint32_t x, uint32_t y, uint32_t z);
	static uint32_t dataIndex(uvec3 pos);
	static uint32_t dataIndex(ivec3Ref pos);
	Chunk(uint32_t id, ivec3 position, ivec3 worldPosition);
	//~Chunk();
	uint8_t* getData();
	uint8_t getBlock(ivec3 pos);
	void setBlock(ivec3 pos, uint8_t block);
	uint32_t getMeshID();
	ivec3 getPosition();
	ivec3 getWorldPosition();
	void setWorldPosition(ivec3 wp);
	void worldMove(ivec3 dt);
	Chunk** getNeighbors();
	void setNeighbor(int side, Chunk* neighbor);
	bool properMesh = false;
 private:
	uint8_t* data;
	//Chunk* neighbors[6];
	uint32_t meshID;
	ivec3 position;
	ivec3 worldPosition;
};

