#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "glmSupport.h"

class RigidBody{
public:
	glm::vec3 force;
	glm::vec3 torque;

	glm::vec3 p;
	glm::vec3 v;
	float mass;

	glm::quat q;
	glm::vec3 omega;

	glm::mat3 I;
	glm::mat3 Iinv;

	RigidBody(float mass, glm::mat3 inertialTensor);

	void addForce(glm::vec3 f, glm::vec3 loc);
	void resolveForces(float dt);
	glm::mat4 matrix();
};