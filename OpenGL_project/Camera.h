#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace glm;
class Camera
{
public:
	Camera();
	Camera(float aspectRatio);
	Camera(float aspectRatio, float viewAngle);
	mat4 getProjection();
	mat4 getView();
	vec3 getPosition();
	float getAspectRatio();
	float getViewAngleDeg();
	vec3 getDirection();
	mat4 getRotation();
	void setAspectRatio(float ratio);
	void setViewAngleDeg(float angleDeg);
	void setPosition(vec3 position);
	void rotateRel(float angleDeg, vec3 axis);
	void rotateAbs(float angleDeg, vec3 axis);
	void translateRel(vec3 translation);
	void translateAbs(vec3 translation);
	void updateProjection();
	void setProjection(float aspectRatio, float viewAngle);
private:
	mat4 projection;
	vec3 position;
	mat4 rotation;
	float viewAngleDeg;
	float aspectRatio;
};

