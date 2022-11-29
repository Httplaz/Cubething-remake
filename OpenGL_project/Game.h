#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <lodepng.h>
#include "Shader.h"
#include "Texture.h"
#include "Vertexpool.h"
#include "Camera.h"
#include "Skybox.h"
#include "WorldGenerator.h"
#include "MeshBuilder.h"
#include "ChunkHolder.h"
#include "ResourseManager.h"
#include "ModelRenderer.h"
#include "WorldInteraction.h"

using namespace std;
using namespace glm;

class Game
{
public:
	Game();
	void render();
	void update();
	void updateAspectRatio(float ratio);
	void setMovementInput(vec3 input);
	void setRotationInput(vec2 input);
	void addMovementInput(vec3 input);
	void addRotationInput(vec2 input);
	void addMouseButtonInput(ivec2 input);
	void terminate();
private:
	Shader shader;
	Camera camera;
	Vertexpool<CompactVertex, MeshAttribPack>* vertexpool;
	ChunkHolder* chunkholder;
	Skybox skybox;
	mat4 model;
	int gameTime;
	ModelRenderer<vec3, vec3>* modelRenderer;

	vec3 movementInput;
	vec2 rotationInput;
	ivec2 mouseButtonInput;

};

