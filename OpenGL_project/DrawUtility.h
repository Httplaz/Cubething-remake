#pragma once
#include <glm/glm.hpp>

typedef uint32_t dataSize_t;

struct DrawCommand
{
	uint32_t verticesCount;
	uint32_t instanceCount = 1;
	uint32_t beginFromVertex;
	uint32_t beginFromInstance;
};

struct DrawElementsCommand
{
	uint32_t verticesCount;
	uint32_t instanceCount = 1;
	uint32_t beginFromIndex;
	uint32_t beginFromVertex;
	uint32_t beginFromInstance;
};

struct Attribute
{
	uint32_t repCount;
	uint32_t glType;
	dataSize_t typeSize;
	bool floatingPoint;
};

