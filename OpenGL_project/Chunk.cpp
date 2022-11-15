#include "Chunk.h"
//#include <iostream>

uint32_t Chunk::dataIndex(uint32_t x, uint32_t y, uint32_t z)
{
	return z * area + y * side + x;
}

uint32_t Chunk::dataIndex(uvec3 pos)
{
	return dataIndex(pos.x, pos.y, pos.z);
}

uint32_t Chunk::dataIndex(ivec3Ref pos)
{
	return dataIndex(*pos.x, *pos.y, *pos.z);
}

Chunk::Chunk(uint32_t id, ivec3 position, ivec3 worldPosition): data(new uint8_t[volume]), meshID(id), position(position), worldPosition(worldPosition)
{
	//for (int i = 0; i < 6; i++)
		//neighbors[i] = nullptr;
	//std::cout << "imachunk " << id << "\n";
}

//Chunk::~Chunk()
//{
	//delete[] data;
//}

uint8_t* Chunk::getData()
{
	return data;
}

uint8_t Chunk::getBlock(ivec3 pos)
{
	return data[dataIndex(pos)];
}

void Chunk::setBlock(ivec3 pos, uint8_t block)
{
	data[dataIndex(pos)] = block;
}

uint32_t Chunk::getMeshID()
{
	return meshID;
}

ivec3 Chunk::getPosition()
{
	return position;
}

ivec3 Chunk::getWorldPosition()
{
	return worldPosition;
}

void Chunk::setWorldPosition(ivec3 wp)
{
	worldPosition = wp;
}

void Chunk::worldMove(ivec3 dt)
{
	worldPosition += dt;
}

Chunk** Chunk::getNeighbors()
{
	return nullptr;// neighbors;
}

void Chunk::setNeighbor(int side, Chunk* neighbor)
{
	//neighbors[side] = neighbor;
}
