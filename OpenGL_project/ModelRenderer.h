#pragma once
#include <memory>
#include <vector>
#include <deque>
#include <map>
#include <utility>
#include "Shader.h"
#include <mutex>
#include "DrawUtility.h"
#include <set>
template<typename vT, typename iaT>
class ModelRenderer
{
public:
	ModelRenderer(uint32_t maxVertexCount, std::vector<Attribute> vertexAttribs, std::vector<Attribute> instanceAttribs);
	void render();
	void update();
	const uint32_t addModel(std::vector<vT>& vertices, std::vector<uint32_t>& indices);
	const uint32_t addModelInstance(const uint32_t modelID);
	void setInstanceAttribute(const uint32_t modelID, const uint32_t instanceId, iaT& data);
	void setShaderProgram(Shader shader);
private:
	std::vector<DrawElementsCommand> drawCommands;
	std::vector<iaT> instanceAttributes;
	///std::map<uint32_t, DrawElementsCommand*> drawCommands;
	uint32_t VAO, VBO, EBO, IBO, ABO;
	Shader shaderProgram;
	uint32_t savedVertices = 0, savedInstances = 0, savedIndices = 0;
	const uint32_t maxVertexCount;
	void* vertexBufferData;
	void* indexBufferData;
	void* indirectBufferData;
	void* attributeBufferData;
	uint32_t sexVAO, sexVBO, sexEBO;
};

template<typename vT, typename iaT>
inline ModelRenderer<vT, iaT>::ModelRenderer(uint32_t vertexCount, std::vector<Attribute> vertexAttribs, std::vector<Attribute> instanceAttribs) : maxVertexCount(vertexCount)
{
	/*glGenBuffers(1, &sexEBO);


	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLbitfield flag = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, maxVertexCount*sizeof(vT), NULL, flag);
	vertexBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, maxVertexCount * sizeof(vT), flag);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ARRAY_BUFFER, EBO);
	glBufferStorage(GL_ARRAY_BUFFER, maxVertexCount * sizeof(uint32_t), NULL, flag);
	indexBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, maxVertexCount * sizeof(uint32_t), flag);

	glGenBuffers(1, &ABO);
	glBindBuffer(GL_ARRAY_BUFFER, ABO);
	glBufferStorage(GL_ARRAY_BUFFER, maxVertexCount * sizeof(iaT)/4, NULL, flag);
	attributeBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, maxVertexCount * sizeof(iaT)/4, flag);


	uint32_t vertexAttributesCount = vertexAttribs.size();
	*/
	float verticesX[] = {
	   -0.5f, -0.5f, 0.0f, // left
		0.5f, -0.5f, 0.0f, // right
		0.0f,  0.5f, 0.0f  // top
	};

	uint32_t indicesX[] = { 0,1,2 };

	std::vector<glm::vec3> vertices =
	{
		{-1, -1, 0},
		{1, -1, 0},
		{1, 1, 0},
		{-1, 1, 0},
		{-1, -1, 1},
		{1, -1, 1},
		{1, 1, 1},
		{-1, 1, 1},
	};
	std::vector<uint32_t> indices =
	{
		0, 1, 2,  2, 3, 0,
		1, 5, 6,  6, 2, 1,
		5, 4, 7,  7, 6, 5,
		4, 0, 4,  4, 7, 4,
		3, 2, 6,  6, 7, 3,
		5, 4, 0,  0, 1, 5
	};
	std::vector<uint32_t> commands =
	{
		{36, 1, 0, 0, 0},
	};

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLbitfield flag = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, maxVertexCount * sizeof(vT), NULL, flag);
	vertexBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, maxVertexCount * sizeof(vT), flag);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, maxVertexCount * sizeof(uint32_t), NULL, flag);
	indexBufferData = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, maxVertexCount * sizeof(uint32_t), flag);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBufferStorage(GL_DRAW_INDIRECT_BUFFER, maxVertexCount / 3 * sizeof(DrawElementsCommand), NULL, flag);
	indirectBufferData = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, maxVertexCount / 3 * sizeof(DrawElementsCommand), flag);

	glBindVertexArray(VAO);

	//glGenBuffers(1, &ABO);
	//glBindBuffer(GL_ARRAY_BUFFER, ABO);
	//glBufferStorage(GL_ARRAY_BUFFER, maxVertexCount / 3 * sizeof(iaT), NULL, flag);
	//attributeBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, maxVertexCount / 3 * sizeof(iaT), flag);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//memcpy(vertexBufferData, vertices.data(), sizeof(vT) * vertices.size());

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//memcpy(indexBufferData, indices.data(), sizeof(uint32_t) * vertices.size());

	//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	//memcpy(indirectBufferData, commands.data(), sizeof(DrawElementsCommand) * commands.size());

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

	glGenBuffers(1, &ABO);
	glBindBuffer(GL_ARRAY_BUFFER, ABO);
	glBufferStorage(GL_ARRAY_BUFFER, maxVertexCount / 3 * sizeof(iaT), NULL, flag);
	attributeBufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, maxVertexCount / 3 * sizeof(iaT), flag);


	uint32_t vertexAttributesCount = vertexAttribs.size();
	attribOffset = 0;
	for (int i = 0; i < instanceAttribs.size(); i++)
	{
		if (instanceAttribs[i].floatingPoint)
			glVertexAttribPointer(vertexAttributesCount + i, instanceAttribs[i].repCount, instanceAttribs[i].glType, GL_FALSE, sizeof(iaT), (void*)attribOffset);
		else
			glVertexAttribIPointer(vertexAttributesCount + i, instanceAttribs[i].repCount, instanceAttribs[i].glType, sizeof(iaT), (void*)attribOffset);

		glVertexAttribDivisor(vertexAttributesCount + i, 1);
		glEnableVertexAttribArray(vertexAttributesCount + i);
		attribOffset += instanceAttribs[i].repCount * instanceAttribs[i].typeSize;
	}


	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


}

template<typename vT, typename iaT>
inline void ModelRenderer<vT, iaT>::render()
{
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBindBuffer(GL_ARRAY_BUFFER, sexVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verticesX), verticesX, GL_STATIC_DRAW);


	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shaderProgram.use();
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, 1, 0);



	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	// glBindVertexArray(0); // no nee
	/*glBindVertexArray(sexVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vT)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sexEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*indices.size(), indices.data(), GL_DYNAMIC_DRAW);
	//memcpy(vertexBufferData, vertices.data(), vertices.size() * sizeof(vT));
	//memcpy(indexBufferData, indices.data(), indices.size() * sizeof(uint32_t));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	//glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, drawCommands.size(), 0);
	glBindVertexArray(0);*/
}

template<typename vT, typename iaT>
inline void ModelRenderer<vT, iaT>::update()
{
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	memcpy(indirectBufferData, drawCommands.data(), sizeof(DrawElementsCommand) * drawCommands.size());
}

template<typename vT, typename iaT>
inline const uint32_t ModelRenderer<vT, iaT>::addModel(std::vector<vT>& vertices, std::vector<uint32_t>& indices)
{
	uint32_t newModelID = drawCommands.size();
	drawCommands.push_back({uint32_t(indices.size()), 0, savedIndices, savedVertices, savedInstances});
	//drawCommands.push_back({ 36, 1, 0, 0, 0});
	memcpy(vertexBufferData, vertices.data(), vertices.size() * sizeof(vT));
	memcpy(indexBufferData, indices.data(), indices.size() * sizeof(uint32_t));
	savedVertices += vertices.size();
	savedIndices += indices.size();
	return newModelID;
}

template<typename vT, typename iaT>
inline const uint32_t ModelRenderer<vT, iaT>::addModelInstance(const uint32_t modelID)
{
	drawCommands[modelID].instanceCount++;
	for (int i = modelID + 1; i < drawCommands.size(); i++)
		drawCommands[i].beginFromInstance++;
	instanceAttributes.insert(instanceAttributes.begin() + modelID, iaT());
	return drawCommands[modelID].instanceCount-1;
}

template<typename vT, typename iaT>
inline void ModelRenderer<vT, iaT>::setInstanceAttribute(const uint32_t modelID, const uint32_t instanceId, iaT& data)
{
	uint32_t absInstanceNumber = drawCommands[modelID].beginFromInstance + instanceId;
	memcpy((char*)attributeBufferData + sizeof(iaT) * absInstanceNumber, &data, sizeof(iaT));
}

template<typename vT, typename iaT>
inline void ModelRenderer<vT, iaT>::setShaderProgram(Shader shader)
{
	shaderProgram = shader;
}
