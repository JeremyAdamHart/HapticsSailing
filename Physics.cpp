#include "Physics.h"
#include <stdio.h>

using namespace glm;

const float DAMPING_LINEAR = 2.0f;
const float DAMPING_ANGULAR = 0.01f;
const vec3 GRAVITY(0, -9.81, 0);

RigidBody::RigidBody(float mass, mat3 inertialTensor) :force(0.f), torque(0.f), p(0.f), v(0.f), 
	mass(mass), q(quat()), omega(0.f), I(inertialTensor), Iinv(inverse(inertialTensor))
{

}

void RigidBody::addForce(vec3 f, vec3 loc){
	vec3 r = loc - p;
	force += dot(f, r)*r / dot(r, r);
	torque += cross(r, f);
}

void RigidBody::resolveForces(float dt){

	printf("Force(%f, %f, %f)\n", force.x, force.y, force.z);

	//Linear integration
	v += ((force - v*DAMPING_LINEAR)/ mass + GRAVITY)*dt;
	p += v*dt;

	//Rotational integration
	mat3 IinvWorld = mat3_cast(q)*Iinv*transpose(mat3_cast(q));
	vec3 ddt_omega = IinvWorld*torque;
	omega += (ddt_omega - DAMPING_ANGULAR*omega)*dt;
	quat omegaQ (0, omega.x, omega.y, omega.z);

	q += dt*0.5f*omegaQ*q;
	q = normalize(q);

	force = vec3(0.0);
	torque = vec3(0.0);
}

mat4 RigidBody::matrix(){ return translateMatrix(p)*mat4_cast(q); }