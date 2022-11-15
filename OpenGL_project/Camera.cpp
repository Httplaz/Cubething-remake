#include "Camera.h"
#include <GLFW/glfw3.h>

Camera::Camera() : Camera(1.f)
{
}

Camera::Camera(float aspectRatio) : Camera(aspectRatio, 90.f)
{
}

Camera::Camera(float aspectRatio, float viewAngleDeg): aspectRatio(aspectRatio), viewAngleDeg(viewAngleDeg)
{
	//view = mat4(1.f);
	setProjection(aspectRatio, viewAngleDeg);
	position = vec3(0);
	rotation = mat4(1);
}

mat4 Camera::getProjection()
{
	return projection;
}

mat4 Camera::getView()
{
	return translate(scale(rotation, vec3(-1)), position*vec3(1,1,1));
}

vec3 Camera::getPosition()
{
	return position*vec3(1,1,-1);
}

float Camera::getAspectRatio()
{
	return aspectRatio;
}
float Camera::getViewAngleDeg()
{
	return viewAngleDeg;
}

vec3 Camera::getDirection()
{
	return vec3(vec4(0, 0, 1, 0) * scale(rotation, vec3(-1,-1,1)));
}

mat4 Camera::getRotation()
{
	return rotation;
}

void Camera::setAspectRatio(float ratio)
{
	aspectRatio = ratio;
	updateProjection();
}

void Camera::setViewAngleDeg(float angleDeg)
{
	viewAngleDeg = angleDeg;
	updateProjection();
}


void Camera::setPosition(vec3 pos)
{
	position = pos*vec3(1, 1, -1);
}

void Camera::rotateRel(float angleDeg, vec3 axis)
{
	rotation = rotate(rotation, glm::radians(angleDeg), vec3(vec4(-axis.x, axis.y, axis.z, 0)*rotation));
}

void Camera::rotateAbs(float angleDeg, vec3 axis)
{
	rotation = glm::rotate(rotation, glm::radians(angleDeg), glm::vec3(axis));
}

void Camera::translateRel(vec3 translation)
{
	position += vec3(vec4(translation*vec3(1,1,-1), 0) * scale(rotation, vec3(1,1,1)));
}

void Camera::translateAbs(vec3 translation)
{
	position += translation*vec3(1,1,-1);
}

void Camera::updateProjection()
{
	projection = perspective(glm::radians(viewAngleDeg / 2.f), aspectRatio, 0.04f, 500.0f);
}

void Camera::setProjection(float aspectRatioArg, float viewAngleArg)
{
	aspectRatio = aspectRatioArg;
	viewAngleDeg = viewAngleArg;
	updateProjection();
}
