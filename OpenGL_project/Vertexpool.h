#pragma once
#include <memory>
#include <vector>
#include <deque>
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include "Shader.h"
#include <mutex>
#include "DrawUtility.h"
#include <vector>
using namespace std;

typedef uint32_t dataSize_t;

struct SectionInfo
{
	uint32_t sectionOffset;
	uint32_t byteSize;
	bool enabled;
};

const uint32_t maxSectionVertices = 32768;

template<typename vT, typename sT>
class Vertexpool
{
public:
	Vertexpool(int portionCount, int portionVertexSize, vector<Attribute> vertexAttribs, vector<Attribute> portionAttribs);
	void render();
	void update();
	void setShaderProgram(Shader shaderProgram);
	bool startPortionMeshing(const uint32_t group);
	bool restartPortionMeshing(const uint32_t group);
	bool placeVertex(const uint32_t group, dataSize_t offset, const vT& data);
	bool placeVertices(const uint32_t group, dataSize_t offset, const std::vector<vT>& data, const uint32_t count);
	bool placeAttribs(const uint32_t group, const sT& data);
	void unmeshPortion(const uint32_t group);
	void enableMesh(const uint32_t group);
	void disableMesh(const uint32_t group);
private:
	recursive_mutex poolMutex;
	void clearPortion(uint32_t bufferOffset);
	vector<DrawCommand> usedDrawCommandsV;
	map<uint32_t, SectionInfo> meshedSections;
	deque<uint32_t> freePortions;
	const uint32_t portionVertexSize, portionCount;
	const uint32_t sectionSize;
	uint32_t VAO, VBO, IBO, meshesDataBO;
	Shader shaderProgram;
	uint32_t drawMode;
	void* vertexBufferData;
	void* portionAttribBufferData;
	uint32_t vertexAttributesCount;
};

template<typename vT, typename sT>
Vertexpool<vT, sT>::Vertexpool(int portionCount, int portionVertexSize, vector<Attribute> vertexAttribs, vector<Attribute> portionAttribs) :
	portionCount(portionCount), portionVertexSize(portionVertexSize),
	freePortions(), sectionSize(portionVertexSize*sizeof(vT))
{
	drawMode = GL_TRIANGLES;
	drawMode = GL_POINTS;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGenBuffers(1, &meshesDataBO);

	GLbitfield flag = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, portionCount * portionVertexSize * sizeof(vT), NULL, flag);
	vertexBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, portionCount * portionVertexSize * sizeof(vT), flag);
	memset(vertexBufferData, 0, portionCount * portionVertexSize * sizeof(vT));




	dataSize_t attribOffset = 0;
	for (int i = 0; i < vertexAttribs.size(); i++)
	{
		if (vertexAttribs[i].floatingPoint)
			glVertexAttribPointer(i, vertexAttribs[i].repCount, vertexAttribs[i].glType, GL_FALSE, sizeof(vT), (void*)attribOffset);
		else
			glVertexAttribIPointer(i, vertexAttribs[i].repCount, vertexAttribs[i].glType, sizeof(vT), (void*)attribOffset);
		glEnableVertexAttribArray(i);
		attribOffset += vertexAttribs[i].repCount * vertexAttribs[i].typeSize;
	}

	vertexAttributesCount = vertexAttribs.size();

	glBindBuffer(GL_ARRAY_BUFFER, meshesDataBO);
	glBufferStorage(GL_ARRAY_BUFFER, portionCount * sizeof(sT), NULL, flag);
	portionAttribBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, portionCount * sizeof(sT), flag);
	attribOffset = 0;
	for (int i = 0; i < portionAttribs.size(); i++)
	{
		if (portionAttribs[i].floatingPoint)
			glVertexAttribPointer(vertexAttributesCount + i, portionAttribs[i].repCount, portionAttribs[i].glType, GL_FALSE, sizeof(sT), (void*)attribOffset);
		else
			glVertexAttribIPointer(vertexAttributesCount + i, portionAttribs[i].repCount, portionAttribs[i].glType, sizeof(sT), (void*)attribOffset);

		glVertexAttribDivisor(vertexAttributesCount + i, 1);
		glEnableVertexAttribArray(vertexAttributesCount + i);
		attribOffset += portionAttribs[i].repCount * portionAttribs[i].typeSize;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	for (uint32_t offset = 0; offset < portionCount; offset++)
	{
		freePortions.push_back(offset);
	}
}


template<typename vT, typename sT>
void Vertexpool<vT, sT>::render()
{
	poolMutex.lock();
	shaderProgram.use();
	glBindVertexArray(VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glMultiDrawArraysIndirect(drawMode, NULL, usedDrawCommandsV.size(), sizeof(DrawCommand));
	glBindVertexArray(0);
	poolMutex.unlock();
}

template<typename vT, typename sT>
void Vertexpool<vT, sT>::update()
{
	poolMutex.lock();
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, usedDrawCommandsV.size() * sizeof(DrawCommand), usedDrawCommandsV.data(), GL_DYNAMIC_DRAW);
	poolMutex.unlock();
}

template<typename vT, typename sT>
void Vertexpool<vT, sT>::setShaderProgram(Shader shaderProgram)
{
	Vertexpool<vT, sT>::shaderProgram = shaderProgram;
}

template<typename vT, typename sT>
bool Vertexpool<vT, sT>::startPortionMeshing(const uint32_t group)
{
	poolMutex.lock();
	if (freePortions.size() == 0) 
	{
		poolMutex.unlock();
		return false;
	}
	uint32_t meshingOffset = freePortions.front();
	freePortions.pop_front();
	meshedSections.insert({ group, {meshingOffset, 0, false} });
	clearPortion(meshingOffset);
	poolMutex.unlock();
}

template<typename vT, typename sT>
bool Vertexpool<vT, sT>::restartPortionMeshing(const uint32_t group)
{
	poolMutex.lock();
	auto sp = meshedSections.find(group);
	sp->second.byteSize = 0;
	poolMutex.unlock();
	return true;
}

template<typename vT, typename sT>
bool Vertexpool<vT, sT>::placeVertex(const uint32_t group, dataSize_t offset, const vT& data)
{
	poolMutex.lock();
	auto sp = meshedSections.find(group);
	memcpy((char*)vertexBufferData + sp->second.sectionOffset * sectionSize + offset, &data, sizeof(vT));
	sp->second.byteSize = offset + sizeof(vT);
	poolMutex.unlock();
	return true;
}

template<typename vT, typename sT>
inline bool Vertexpool<vT, sT>::placeVertices(const uint32_t group, dataSize_t offset, const std::vector<vT>& data, const uint32_t count)
{
	poolMutex.lock();
	auto sp = meshedSections.find(group);
	memcpy((char*)vertexBufferData + sp->second.sectionOffset * sectionSize + offset, data.data(), sizeof(vT) * count);
	sp->second.byteSize = offset + sizeof(vT) * count;
	poolMutex.unlock();
	return true;
}

template<typename vT, typename sT>
bool Vertexpool<vT, sT>::placeAttribs(const uint32_t group, const sT& data)
{
	poolMutex.lock();
	auto sp = meshedSections.find(group);
	memcpy((char*)(portionAttribBufferData)+sp->second.sectionOffset * sizeof(sT), &data, sizeof(sT));
	poolMutex.unlock();
	return true;
}

template<typename vT, typename sT>
void Vertexpool<vT, sT>::unmeshPortion(const uint32_t key)
{
	disableMesh(key);
	poolMutex.lock();
	auto sp = meshedSections.find(key);
	dataSize_t bufferOffset = sp->second.sectionOffset;
	freePortions.push_back(bufferOffset);
	poolMutex.unlock();
}

template<typename vT, typename sT>
void Vertexpool<vT, sT>::enableMesh(const uint32_t group)
{
	poolMutex.lock();
	auto sp = meshedSections.find(group);
	if (sp->second.enabled)
		disableMesh(group);
	usedDrawCommandsV.push_back({ sp->second.byteSize / sizeof(vT), 1, sp->second.sectionOffset * portionVertexSize, sp->second.sectionOffset });
	sp->second.enabled = true;
	poolMutex.unlock();
}

template<typename vT, typename sT>
void Vertexpool<vT, sT>::disableMesh(const uint32_t group)
{
	poolMutex.lock();
	auto sp = meshedSections.find(group);
	for (auto it = usedDrawCommandsV.begin(); it != usedDrawCommandsV.end(); it++)
		if (it->beginFromInstance == sp->second.sectionOffset)
		{
			usedDrawCommandsV.erase(it);
			sp->second.enabled = false;
			break;
		}
	poolMutex.unlock();
}

template<typename vT, typename sT>
void Vertexpool<vT, sT>::clearPortion(uint32_t bufferOffset)
{
	//memset((char*)vertexBufferData + bufferOffset * portionVertexSize * sizeof(vT), 0, portionVertexSize * sizeof(vT));
	//memset((char*)portionAttribBufferData + bufferOffset * sizeof(sT), 0, sizeof(sT));
}


