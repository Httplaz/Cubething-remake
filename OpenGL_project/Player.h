#pragma once
#include "Camera.h"
#include <glm/glm.hpp>

using namespace glm;

class Player
{
private:
	Camera* camera;
	vec3 movementInput;
	vec2 rotationInput;
	bool flight;
	bool shift;
public:
	Player();
	void update();
	void setRotationInput();
	void setMovementInput();
};

