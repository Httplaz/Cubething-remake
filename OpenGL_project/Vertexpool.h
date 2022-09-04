#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <deque>
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
using namespace std;

struct DrawCommand 
{
	uint32_t verticesCount;
	uint32_t instanceCount;
	size_t beginFrom;
	uint32_t group; //=baseInstance
	vec3 pos;
};

struct Attribute
{
	uint32_t positionCount;
	uint32_t glType;
	size_t typeSize;
};

struct MeshAttribute
{
	uint32_t positionCount;
	uint32_t glType;
	size_t typeSize;
};


struct MeshingProcessInfo
{
	uint32_t meshingPortionOffset;
	size_t dataPortionFillOffset;
	size_t attribPortionFillOffset;
};


class Vertexpool
{
public:
	Vertexpool(int portionCount, int portionVertexSize, vector<Attribute> attributes);
	void setPortionAttributes(vector<Attribute> attributes);
	void render();
	void update();
	void setShaderprogram(shared_ptr<Shader> shaderProgram);
	bool meshPortion(const uint32_t group, void* data, bool enable);
	bool meshPortion(const uint32_t group, void* data);
	bool startPortionMeshing(const uint32_t group);
	bool placeVertex(const uint32_t group, const void* data);
	bool placeData(const uint32_t group, const void* data, size_t size);
	bool placeAttribs(const uint32_t group, const void* data, size_t size);
	void endPortionMeshing(const uint32_t group, bool enable);
	void endPortionMeshing(const uint32_t group);
	void unmeshPortion(const uint32_t group);
	void enableMesh(const uint32_t group);
	void disableMesh(const uint32_t group);
	void cullFaceMask(uint32_t groupCount, vec3* groupNormales, vec3 camDirection, mat4 view);
private:
	bool inFrustrum(mat4 view, vec3 a, vec3 b);
	bool seen(vec3 camDirection, vec3 normale);
	void disableMesh(const uint32_t key, map<uint32_t, pair < DrawCommand, bool> > ::iterator p);
	void clearPortion(uint32_t bufferOffset);
	vector<DrawCommand> drawCommands;
	uint32_t maskedDrawCommandCount;
	map<uint32_t, pair< DrawCommand, bool > > meshedPortions;
	map<uint32_t, MeshingProcessInfo > meshingProcess;
	deque<uint32_t> freePortions;
	const uint32_t portionVertexSize, portionCount;
	const size_t vertexSize;
	size_t attribPackSize;
	uint32_t VAO, VBO, IBO, meshesDataBO;
	shared_ptr<Shader> shaderProgram;
	uint32_t drawMode;
	void* vertexBufferData;
	void* portionAttribBufferData;
	uint32_t vertexAttributesCount;
};

