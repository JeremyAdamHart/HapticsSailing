#include "Physics.h"
#include <stdio.h>
#include <Eigen/Core>

using namespace glm;

RigidBody::RigidBody(float mass, mat3 inertialTensor) :force(0.f), torque(0.f), p(0.f), v(0.f), 
	mass(mass), q(quat()), omega(0.f), I(inertialTensor), Iinv(inverse(inertialTensor))
{

}

RigidBody::RigidBody(float mass, MeshInfoLoader *geometry) :force(0.f), torque(0.f), p(0.f), v(0.f),
mass(mass), q(quat()), omega(0.f)
{

//	I = calculateInertialTensor(geometry);		//Assumes center of mass is at origin
	Iinv = inverse(I);
}

void RigidBody::addForce(vec3 f, vec3 loc){
	vec3 r = loc - p;
	force += f;	// dot(f, r)*r / dot(r, r);
	torque += cross(r, f);
}

void RigidBody::resolveForces(float dt){

	//Linear integration
	v += force/mass*dt;
	p += v*dt;

	//Rotational integration
	mat3 IinvWorld = mat3_cast(q)*Iinv*transpose(mat3_cast(q));
	vec3 ddt_omega = IinvWorld*torque;
	omega += ddt_omega*dt;
	quat omegaQ (0, omega.x, omega.y, omega.z);

	q += dt*0.5f*omegaQ*q;
	q = normalize(q);

	force = vec3(0.0);
	torque = vec3(0.0);
}

mat4 RigidBody::matrix(){ return translateMatrix(p)*mat4_cast(q); }

void calculateMeshMassPoints(MeshInfoLoader *geometry, vector<float> *masses, vector<vec3> *positions, int numPoints){
	//Generate random points along mesh

	//Solve for mass values that produce origin as center of mass

}

mat3 calculateInertialTensor(MeshInfoLoader *geometry, float mass){
	Eigen::Matrix<float, 20, 1> vec;
	Eigen::Matrix<float, 20, 20> mat;

	Eigen::Matrix<float, 20, 1> v2 = mat*vec;

	return mat3();
}