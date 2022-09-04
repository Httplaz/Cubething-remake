#pragma once
#define GLFW_INCLUDE_NONE

// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;

class Chunk
{ 
public:
	static const uint32_t side = 32;
	static const uint32_t area = side * side;
	static const uint32_t volume = side * side * side;
	static uint32_t dataIndex(uint32_t x, uint32_t y, uint32_t z);
	static uint32_t dataIndex(uvec3 pos);
	Chunk(uint32_t id, ivec3 position);
	//~Chunk();
	uint8_t* getData();
	uint32_t getMeshID();
	ivec3 getPosition();
	Chunk** getNeighbors();
	void setNeighbor(int side, Chunk* neighbor);
 private:
	uint8_t* data;
	Chunk* neighbors[6];
	uint32_t meshID;
	ivec3 position;
};
