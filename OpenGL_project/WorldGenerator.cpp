#include "WorldGenerator.h"

void WorldGenerator::fillChunk(Chunk* chunk)
{
	ivec3 pos = chunk->getWorldPosition()*int32_t(Chunk::side);
	uint8_t* const data = chunk->getData();
	for (int x = 0; x < Chunk::side; x++)
		for (int z = 0; z < Chunk::side; z++)
			for (int y = 0; y < Chunk::side; y++)
				//data[Chunk::dataIndex(x, y, z)] = (y+pos.y > 20 || y<0 || x>31 || x<0 || z>31 || z<0 ? 0 : rand()%20);

				data[Chunk::dataIndex(x, y, z)] = (y + pos.y > (sin(radians((x + pos.x) * (2.f))) + 1) * 6 * (sin(radians((z + pos.z) * 1.2f)) + 1) || y < 0 || x>31 || x < 0 || z>31 || z < 0 ? 0 : (((y + pos.y)) %4 + 1));
	
}
