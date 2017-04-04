#pragma once

#include "glm/glm.hpp"
#include "Camera.h"

class SimpleCamera : public Camera{
public:
	glm::vec3 dir;
	glm::vec3 right;
	glm::vec3 up;

	glm::vec3 pos;

	glm::mat4 projection;

	SimpleCamera();

	SimpleCamera(glm::vec3 _dir, glm::vec3 _up, glm::vec3 _pos, const glm::mat4 &projection = glm::mat4());

	glm::mat4 getCameraMatrix() const;
	glm::mat4 getProjectionMatrix() const;
	glm::vec3 getPosition() const;
};