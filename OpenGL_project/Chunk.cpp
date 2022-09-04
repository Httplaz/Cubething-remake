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

Chunk::Chunk(uint32_t id, ivec3 position): data(new uint8_t[volume]), meshID(id), position(position)
{
	for (int i = 0; i < 6; i++)
		neighbors[i] = nullptr;
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

uint32_t Chunk::getMeshID()
{
	return meshID;
}

ivec3 Chunk::getPosition()
{
	return position;
}

Chunk** Chunk::getNeighbors()
{
	return neighbors;
}

void Chunk::setNeighbor(int side, Chunk* neighbor)
{
	neighbors[side] = neighbor;
}