#include "Vertexpool.h"


size_t getAttributePackSize(vector<Attribute>::iterator begin, vector<Attribute>::iterator end)
{
	size_t size = 0;
	while (begin != end)
	{
		size += (*begin).typeSize*(*begin).positionCount;
		begin++;
	}
	return size;
}

Vertexpool::Vertexpool(int portionCount, int portionVertexSize, vector<Attribute> attributes): 
portionCount(portionCount), portionVertexSize(portionVertexSize), drawCommands(), meshedPortions(), 
freePortions(), vertexSize(getAttributePackSize(attributes.begin(), attributes.end()))
{
	drawMode = GL_TRIANGLES;
	//drawMode = GL_LINE_STRIP;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &meshesDataBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	const GLbitfield flag = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, portionCount * portionVertexSize * vertexSize, NULL, flag);
	vertexBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, portionCount * portionVertexSize * vertexSize, flag);
	size_t attribOffset = 0;
	for (int i = 0; i < attributes.size(); i++)
	{
		glVertexAttribPointer(i, attributes[i].positionCount, attributes[i].glType, GL_FALSE, vertexSize, (void*)attribOffset);
	    glEnableVertexAttribArray(i);
		attribOffset += attributes[i].positionCount * attributes[i].typeSize;
	}
	
	vertexAttributesCount = attributes.size();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	for (uint32_t offset = 0; offset < portionCount; offset++)
	{
		freePortions.push_back(offset);
	}
}

void Vertexpool::setPortionAttributes(vector<Attribute> attributes)
{
	attribPackSize = getAttributePackSize(attributes.begin(), attributes.end());
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, meshesDataBO);
	const GLbitfield flag = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, portionCount * attribPackSize, NULL, flag);
	portionAttribBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, portionCount  * attribPackSize, flag);
	size_t attribOffset = 0;
	for (int i = 0; i < attributes.size(); i++)
	{
		glVertexAttribPointer(vertexAttributesCount+i, attributes[i].positionCount, attributes[i].glType, GL_FALSE, attribPackSize, (void*)attribOffset);
		glVertexAttribDivisor(vertexAttributesCount + i, 1);
		glEnableVertexAttribArray(vertexAttributesCount + i);
		attribOffset += attributes[i].positionCount * attributes[i].typeSize;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Vertexpool::render()
{
	shaderProgram.get()->use();
	glBindVertexArray(VAO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	//glBufferData(GL_DRAW_INDIRECT_BUFFER, drawCommands.size() * sizeof(DrawCommand), drawCommands.data(), GL_DYNAMIC_DRAW);
	glMultiDrawArraysIndirect(drawMode, NULL, maskedDrawCommandCount, sizeof(DrawCommand));
	glBindVertexArray(0);
}

void Vertexpool::update()
{
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, maskedDrawCommandCount * sizeof(DrawCommand), drawCommands.data(), GL_DYNAMIC_DRAW);
}

void Vertexpool::setShaderprogram(shared_ptr<Shader> shaderProgram)
{
	Vertexpool::shaderProgram = shaderProgram;
}

bool Vertexpool::meshPortion(const uint32_t key, void* data)
{
	return Vertexpool::meshPortion(key, data, true);
}

bool Vertexpool::meshPortion(const uint32_t key, void* data, bool enable)
{
	if (freePortions.size() == 0)
		return false;
	size_t bufferOffset = freePortions.front();
	//clearPortion(bufferOffset);
	memcpy((char*)vertexBufferData+bufferOffset, (char*)data, portionVertexSize * vertexSize);
	meshedPortions.insert({ key,{ {portionVertexSize, 1, bufferOffset / vertexSize, key }, false } });
	if (enable)
		enableMesh(key);
	freePortions.pop_front();
	return true;
}

bool Vertexpool::startPortionMeshing(const uint32_t group)
{
	if (freePortions.size() == 0)
		return false;
	uint32_t meshingOffset = freePortions.front();
	freePortions.pop_front();
	clearPortion(meshingOffset);
	size_t portionFill = 0;
	meshingProcess.insert({ group, { meshingOffset, portionFill, portionFill } });
}

bool Vertexpool::placeVertex(const uint32_t group, const void* data)
{
	return placeData(group, data, vertexSize);
}

bool Vertexpool::placeData(const uint32_t group, const void* data, size_t size)
{
	auto p = meshingProcess.find(group);
	if (p->second.dataPortionFillOffset + size > portionVertexSize * vertexSize)
		return false;
	memcpy((char*)(vertexBufferData)+p->second.meshingPortionOffset * portionVertexSize *vertexSize + p->second.dataPortionFillOffset, data, size);
	p->second.dataPortionFillOffset += size;
	return true;
}

bool Vertexpool::placeAttribs(const uint32_t group, const void* data, size_t size)
{

	auto p = meshingProcess.find(group);
	if (p->second.attribPortionFillOffset + size > attribPackSize)
		return false;
	memcpy((char*)(portionAttribBufferData)+p->second.meshingPortionOffset * attribPackSize + p->second.attribPortionFillOffset, data, size);
	p->second.attribPortionFillOffset += size;
	return true;
}

void Vertexpool::endPortionMeshing(const uint32_t group, bool enable)
{
	auto p = meshingProcess.find(group);
	meshedPortions.insert({ group, {{ p->second.dataPortionFillOffset/vertexSize, 1, p->second.meshingPortionOffset * portionVertexSize, group}, false} });
	if (enable)
		enableMesh(group);
}

void Vertexpool::endPortionMeshing(const uint32_t group)
{
	Vertexpool::endPortionMeshing(group, true);
}

void Vertexpool::unmeshPortion(const uint32_t key)
{
	//cout << "\n\n\n";

	//for (auto it = meshedPortions.begin(); it != meshedPortions.end(); it++)
		//cout << it->first << "\n";

	//cout << "\n\n\n";

	auto p = meshedPortions.find(key);
	disableMesh(key, p);
	size_t bufferOffset = p->second.first.beginFrom/portionVertexSize;
	freePortions.push_back(bufferOffset);
	meshedPortions.erase(p);
}

void Vertexpool::enableMesh(const uint32_t key)
{
	maskedDrawCommandCount++;
	auto p = meshedPortions.find(key);
	if (p->second.second)
		return;
	p->second.second = true;
	drawCommands.push_back(p->second.first);
}

void Vertexpool::disableMesh(const uint32_t key)
{
	//cout << "DISABLING MESH\n";
	maskedDrawCommandCount--;
	auto p = meshedPortions.find(key);
	disableMesh(key, p);
}

void Vertexpool::cullFaceMask(uint32_t groupCount, vec3* groupNormales, vec3 camDirection, mat4 view)
{
	maskedDrawCommandCount = drawCommands.size();
	bool sortFinished = false;
	int a = 0, b = drawCommands.size();
	if (b == 0)
		return;
	while (!sortFinished)
	{
		if (!seen(camDirection, groupNormales[drawCommands[a].group%groupCount]))
			swap(drawCommands[a], drawCommands[b-1]), b--;
		else
			a++;
		if (a == b)
			sortFinished = true, maskedDrawCommandCount = b;
	}

	//cout << camDirection.x << " " << camDirection.y << " " << camDirection.z << "\n";
	//cout << degrees(angle(vec3(0, 0, 1), vec3(0, 0, -1))) << "\n";
	//cout << maskedDrawCommandCount << "\n";
	//maskedDrawCommandCount = 5;

}

bool Vertexpool::inFrustrum(mat4 view, vec3 a, vec3 b)
{
	return false;
}

bool Vertexpool::seen(vec3 camDirection, vec3 normale)
{
	//cout << angle(vec3(0, 0, 1), normale) << " ";
	//cout << normale.x << " " << normale.y << " " << normale.z << "\n";
	if(degrees((angle(camDirection, normale))) > 60.f)
	{
		//cout << "fine\n";
		return true;
	}
	return false;
}

void Vertexpool::disableMesh(const uint32_t group, map<uint32_t, pair<DrawCommand, bool> >::iterator p)
{
	if (!p->second.second)
		return;
	//cout << "MY GROUP: " << group << "\n";
	for (auto it = drawCommands.begin(); it != drawCommands.end(); it++)
		if (it->group == group)
		{
			drawCommands.erase(it);
			p->second.second = false;
			//cout << "mesh erased\n";
			break;
		}
		//else
			//cout << it->group << "\n";
}

void Vertexpool::clearPortion(uint32_t bufferOffset)
{
	memset((char*)vertexBufferData + bufferOffset*portionVertexSize*vertexSize, 0, portionVertexSize * vertexSize);
	memset((char*)portionAttribBufferData + bufferOffset*attribPackSize, 0, attribPackSize);
}
