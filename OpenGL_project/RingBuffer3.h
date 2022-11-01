#pragma once
#include <glm/glm.hpp>
#include <iostream>
using namespace glm;



template<typename T>
class RingBuffer3
{
public:
	RingBuffer3(uint32_t side);
	RingBuffer3(ivec3 side);
	uint32_t dataIndex(ivec3 pos);
	const T get(ivec3 pos);
	const T getAbs(ivec3 pos);
	void set(ivec3 pos, T sample);
	void shift(ivec3 delta);
	ivec3 getOffset();
private:
	const ivec3 side;
	const uint32_t areaY = side.x * side.z, areaZ = side.x * side.y;
	const uint32_t volume = areaY * side.y;
	ivec3 offset;
	T* const data;
};



//definitions

template<typename T>
RingBuffer3<T>::RingBuffer3(ivec3 side) : side(side), data(new T[volume]), offset({ 0,0,0 })
{

}

template<typename T>
RingBuffer3<T>::RingBuffer3(uint32_t side) : RingBuffer3({ side, side, side })
{

}

template<typename T>
uint32_t RingBuffer3<T>::dataIndex(ivec3 pos)
{
	return pos.x + pos.y * side.x + pos.z * areaZ;
}

template<typename T>
const T RingBuffer3<T>::get(ivec3 pos)
{
	ivec3 a = (pos - offset + side) % side;
	//std::cout << a.x << " " << a.y << " " << a.z << "\n";
	return data[dataIndex(a)];
}

template<typename T>
const T RingBuffer3<T>::getAbs(ivec3 pos)
{
	ivec3 a = (pos + side) % side;
	//std::cout << a.x << " " << a.y << " " << a.z << "\n";
	return data[dataIndex(a)];
}

template<typename T>
void RingBuffer3<T>::set(ivec3 pos, T sample)
{
	data[dataIndex((pos - offset + side) % side)] = sample;
}

template<typename T>
void RingBuffer3<T>::shift(ivec3 delta)
{
	offset = (offset + delta + side) % side;
}

template<typename T>
ivec3 RingBuffer3<T>::getOffset()
{
	return offset;
}
