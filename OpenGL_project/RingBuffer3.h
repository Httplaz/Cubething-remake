#pragma once
#include <glm/glm.hpp>
using namespace glm;



template<typename T>
class RingBuffer3
{
public:
	RingBuffer3(uint32_t side);
	uint32_t dataIndex(ivec3 pos);
	const T get(ivec3 pos);
	void set(ivec3 pos, T sample);
	void shift(ivec3 delta);
	ivec3 getOffset();
private:
	const int32_t side;
	const int32_t area = side * side;
	const int32_t volume = area * side;
	ivec3 offset;
	T* const data;
};



//definitions

template<typename T>
RingBuffer3<T>::RingBuffer3(uint32_t side) : side(side), data(new T[volume]), offset({ 0,0,0 })
{

}

template<typename T>
uint32_t RingBuffer3<T>::dataIndex(ivec3 pos)
{
	return pos.x + pos.y * side + pos.z * area;
}

template<typename T>
const T RingBuffer3<T>::get(ivec3 pos)
{
	return data[dataIndex((pos+offset+ivec3(side))%side)];
}

template<typename T>
void RingBuffer3<T>::set(ivec3 pos, T sample)
{
	data[dataIndex(pos)] = sample;
}

template<typename T>
void RingBuffer3<T>::shift(ivec3 delta)
{
	offset = (offset + delta + ivec3(side)) % side;
}

template<typename T>
ivec3 RingBuffer3<T>::getOffset()
{
	return offset;
}
