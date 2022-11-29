#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include "ChunkHolder.h"
using namespace glm;
class WorldInteraction
{
public:
	static glm::ivec4 raycast(glm::vec3 pos, glm::vec3 dir, ChunkHolder& holder);
private:
};

