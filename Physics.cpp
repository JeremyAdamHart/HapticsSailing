#include "Physics.h"
#include <stdio.h>
#include <Eigen/Core>
#include <random>
#include <ctime>
#include <glm/gtc/type_ptr.hpp>

const float MINIMUM_FORWARD_VELOCITY = 0.f;

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

void RigidBody::addForceAndRepeat(vec3 f, vec3 loc) {
	vec3 r = loc - p;
	force += f;	// dot(f, r)*r / dot(r, r);
	torque += cross(r, f);

	repeatForces.push_back(f);
	repeatTorques.push_back(torque);
}

void RigidBody::clearRepeat() {
	repeatForces.clear();
	repeatTorques.clear();
}

void RigidBody::addTorqueOnly(vec3 f, vec3 loc) {
	vec3 r = loc - p;
	torque += cross(r, f);
}

void RigidBody::resolveForces(float dt){

	vec3 forward = toVec3(normalize(matrix()*vec4(0, 0, -1, 0)));
	vec3 v_parallel = dot(v, forward)*forward;
	vec3 v_perpendicular = v - v_parallel;

	if(length(v_parallel) < MINIMUM_FORWARD_VELOCITY) 
		v_parallel = forward*MINIMUM_FORWARD_VELOCITY;

	v = v_parallel + v_perpendicular;

	//Linear integration
	v += force/mass*dt;
	p += v*dt;

	//printf("Force (%.2f, %.2f, %.2f)\n", force.x, force.y, force.z);

	//I might have broke something, compare to RenderingEngine Physics.cpp
	//Rotational integration
/*	mat3 IinvWorld = mat3_cast(q)*Iinv*transpose(mat3_cast(q));
	vec3 ddt_omega = torque;
	omega += ddt_omega*dt;

	vec3 angVelocityV = IinvWorld*omega;	//World space
	quat angVelocityQ (0, angVelocityV.x, angVelocityV.y, angVelocityV.z);

	q += dt*0.5f*angVelocityQ*q;
*/

	/* From Physics.cpp

	//Linear integration
	v += (force/mass)*dt;
	position += v*dt;

	//Rotational integration - Body space to world space
	mat3 IinvWorld = mat3_cast(orientation)*Iinv*transpose(mat3_cast(orientation));
	vec3 ddt_omega = torque;		//World space
	omega += ddt_omega*dt;					//World space
	vec3 angVelocityV = IinvWorld*omega;	//World space
	quat angVelocityQ (0, angVelocityV.x, angVelocityV.y, angVelocityV.z);

	orientation += dt*	0.5f*angVelocityQ*orientation;
	orientation = normalize(orientation);

	force = vec3(0.0);
	torque = vec3(0.0);
	
	*/

//I might have broke something, compare to RenderingEngine Physics.cpp
//Rotational integration
	mat3 IinvWorld = mat3_cast(q)*Iinv*transpose(mat3_cast(q));
	vec3 ddt_omega = IinvWorld*torque;

	omega += ddt_omega*dt;
	quat omegaQ(0, omega.x, omega.y, omega.z);

	q += dt*0.5f*omegaQ*q;

	q = normalize(q);

	force = vec3(0.0);
	torque = vec3(0.0);

	for (int i = 0; i < repeatForces.size(); i++) {
		force += repeatForces[i];
	}
	for (int i = 0; i < repeatTorques.size(); i++) {
		torque += repeatTorques[i];
	}
}

mat4 RigidBody::matrix(){ return translateMatrix(p)*mat4_cast(q); }

static float rand01() { return float(rand()) / float(RAND_MAX); }

void calculateMeshMassPoints(MeshInfoLoader *geometry, vector<float> *masses, vector<vec3> *points, int numPoints){
	//Generate random points along mesh
	//Calculate total area
	float totalArea = 0.f;
	vector<float> areas;

	for (int i = 0; i+2 < geometry->indices.size(); i+=3){
		vec3 p1 = geometry->vertices[geometry->indices[i]];
		vec3 p2 = geometry->vertices[geometry->indices[i+1]];
		vec3 p3 = geometry->vertices[geometry->indices[i+2]];
		
		float area = 0.5f*length(cross(p2 - p1, p3 - p1));

		totalArea += area;
		areas.push_back(totalArea);
	}

	float stepSize = totalArea / float(numPoints - 1);
	float a = 0.f;

	srand(time(0));

	int j = 0;
	for (int i = 0; i < numPoints; i++){

		while ((a > areas[j]) && (j < areas.size()-1) ){
			j++;
		}

		if (3 * j + 2 >= geometry->indices.size())
			break;

		vec3 p1 = geometry->vertices[geometry->indices[3*j]];
		vec3 p2 = geometry->vertices[geometry->indices[3*j + 1]];
		vec3 p3 = geometry->vertices[geometry->indices[3*j + 2]];

		float s = rand01();
		float t = rand01();

		if (s + t > 1.f){
			s = 1.f - s;
			t = 1.f - t;
		}

		points->push_back(p1 + (p2 - p1)*s + (p2 - p1)*t);

		a += stepSize;
	}

	//Solve for mass values that produce origin as center of mass
	//Calculate uniform center of mass
	vec3 original_com = vec3(0.f);
	for (int i = 0; i < points->size(); i++){
		original_com += points->at(i) / float(points->size());
	}

	//Create points matrix
	Eigen::Matrix<float, 4, Eigen::Dynamic> P(4, numPoints);

	for (int i = 0; i < points->size(); i++){
		P(0, i) = points->at(i).x;
		P(1, i) = points->at(i).y;
		P(2, i) = points->at(i).z;
		P(3, i) = 1.f;
	}

	Eigen::Matrix<float, 4, 1> c;
	c << -original_com.x, -original_com.y, -original_com.z, 0.f;

	Eigen::Matrix<float, Eigen::Dynamic, 1> m = P.transpose()*(P*P.transpose()).inverse()*c;

	for (int i = 0; i < numPoints; i++){
		masses->push_back(1.f / float(numPoints));	// +m(i, 0));
	}

}

mat3 calculateInertialTensor(MeshInfoLoader *geometry, float mass){
	vector<vec3> points;
	vector<float> masses;

	const int NUM_POINTS = 5000;
	calculateMeshMassPoints(geometry, &masses, &points, NUM_POINTS);

	if (masses.size() != points.size())
		cout << "Error" << endl;

	mat3 inertialTensor(0.f);
	for (int i = 0; i < points.size(); i++){
		vec3 p = points[i];
		float m = masses[i]*mass;
//		mat3 massTensor;
		float tensor[9] = {
			m*(p.y*p.y + p.z*p.z), -m*p.y*p.x, -m*p.z*p.x,
			-m*p.x*p.y, m*(p.x*p.x + p.z*p.z), -m*p.z*p.y,
			-m*p.x*p.z, -m*p.y*p.z, m*(p.x*p.x + p.y*p.y) };
		inertialTensor += make_mat3(tensor);
	}

	return inertialTensor;
}

void RigidBody::addGravityForces() {
	force += GRAVITY*mass;
}

void RigidBody::addDampingForces() {
	vec3 forward = toVec3(matrix()*vec4(0, 0, 1, 0));
	forward.y = 0.f;
	vec3 v_parallel = dot(v, normalize(forward))*forward;
	vec3 v_perpendicular = v - v_parallel;

	force += -v_perpendicular*DAMPING_LINEAR - v*v_parallel*DAMPING_LINEAR_FORWARDS;
	torque += -omega*DAMPING_ANGULAR
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		;
}

//-0.22352  1.26375