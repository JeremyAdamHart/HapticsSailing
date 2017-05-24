#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "glmSupport.h"
#include "MeshInfoLoader.h"

const float DAMPING_LINEAR = 5000.f;
const float DAMPING_LINEAR_FORWARDS = 2.f;
const float DAMPING_ANGULAR = 100000.f;
const glm::vec3 GRAVITY(0, -9.81, 0);

class RigidBody{
public:
	vector<vec3> repeatForces;
	vector<vec3> repeatTorques;

	glm::vec3 force;
	glm::vec3 torque;

	glm::vec3 p;
	glm::vec3 v;
	float mass; 

	glm::quat q;
	glm::vec3 omega;

	glm::mat3 I;
	glm::mat3 Iinv;

	RigidBody(float mass, MeshInfoLoader *geometry);

	RigidBody(float mass, glm::mat3 inertialTensor);

	void addForce(glm::vec3 f, glm::vec3 loc);
	void addTorqueOnly(glm::vec3 f, glm::vec3 loc);
	void addForceAndRepeat(vec3 f, vec3 loc);
	void clearRepeat();
	void resolveForces(float dt);

	void addGravityForces();
	void addDampingForces();
	glm::mat4 matrix();
};

void calculateMeshMassPoints(MeshInfoLoader *geometry, vector<float> *masses, vector<glm::vec3> *positions, int numPoints);
glm::mat3 calculateInertialTensor(MeshInfoLoader *geometry, float mass);